/**
 * Copyright 2011-2015 Matthew Endsley
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <string.h>
#include <vector>
#include <opus.h>
#include <webrtc/modules/audio_processing/include/audio_processing.h>
#include "tiny/audio/capture.h"
#include "tiny/audio/resample.h"
#include "tiny/endian.h"
#include "tiny/voice/engine.h"

using namespace tiny;
using namespace tiny::audio;
using namespace tiny::voice;

struct voice::Source
{
	std::vector<float> incomingData;
	resample::Linear outputResampler;
	OpusDecoder* decoder;
	uint32_t incomingSequence;
};

struct voice::Engine
{
	// number of samples @48k * 10ms
	static const uint32_t c_nmonoBuffer = 480;

	ICaptureDevice* mic;
	OpusEncoder* encoder;
	uint32_t samplesPer10ms;
	uint32_t outputSampleRate;
	uint32_t outgoingSequence;
	int channels;
	resample::Linear micResampler;
	float monoBuffer[c_nmonoBuffer];
};

Engine* voice::engineCreate(ICaptureDevice* microphone, uint32_t sampleRate)
{
	if (!microphone)
		return nullptr;

	int error;
	OpusEncoder* encoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_VOIP, &error);
	if (!encoder)
	{
		return nullptr;
	}

	Engine* e = new Engine;
	e->samplesPer10ms = microphone->samplesPer10ms();
	e->encoder = encoder;
	e->channels = microphone->channels();
	e->mic = microphone;
	e->micResampler.reset(e->mic->sampleRate(), 48000);
	e->outputSampleRate = sampleRate;
	e->outgoingSequence = 0;
	return e;
}

void voice::engineRelease(Engine* e)
{
	opus_encoder_destroy(e->encoder);
	delete e;
}

Source* voice::engineAddSource(Engine* e)
{
	int error;
	OpusDecoder* decoder = opus_decoder_create(48000, 1, &error);
	if (!decoder)
	{
		return nullptr;
	}

	Source* s = new Source;
	s->outputResampler.reset(48000, e->outputSampleRate);
	s->decoder = decoder;
	s->incomingSequence = 0;
	return s;
}

void voice::engineRemoveSource(Engine* /*e*/, Source* s)
{
	opus_decoder_destroy(s->decoder);
	delete s;
}

uint32_t voice::engineGeneratePacket(Engine* e, uint8_t* packet, uint32_t npacket)
{
	if (npacket < 4)
		return 0;

	uint32_t sequence = endianToLittle(e->outgoingSequence);
	memcpy(packet, &sequence, sizeof(sequence));
	packet += sizeof(sequence);
	npacket -= sizeof(sequence);

	uint32_t packetWritten = sizeof(uint32_t);
	uint32_t packetsGenerated = 0;
	while (npacket > 200)
	{
		// do we have 10ms of data to encode?
		const float* incoming = e->mic->get10msOfSamples();
		if (!incoming)
			break;

		if (e->channels == 2)
		{
			e->micResampler.resampleStereoToMono(incoming, e->samplesPer10ms, e->monoBuffer, Engine::c_nmonoBuffer);
		}
		else
		{
			e->micResampler.resampleMono(incoming, e->samplesPer10ms, e->monoBuffer, Engine::c_nmonoBuffer);
		}

		int32_t packetData = opus_encode_float(e->encoder, e->monoBuffer, Engine::c_nmonoBuffer, packet+2, npacket-2);
		if (packetData < 1) // no need to transmit this data
		{
			break;
		}

		uint16_t encodedPacketData = endianToLittle(static_cast<uint16_t>(packetData));
		memcpy(packet, &encodedPacketData, sizeof(encodedPacketData));

		packet += sizeof(encodedPacketData);
		npacket -= sizeof(encodedPacketData);
		packetWritten += sizeof(encodedPacketData);

		npacket -= packetData;
		packet += packetData;
		packetWritten += packetData;
	}

	e->outgoingSequence += packetsGenerated;

	// did we actually write any audio data?
	if (packetWritten == sizeof(uint32_t))
	{
		return 0;
	}

	return packetWritten;
}

void voice::engineProcessPacket(Engine* e, Source* s, const uint8_t* packet, uint32_t npacket)
{
	if (npacket < 6)
		return;

	uint32_t incomingSequence;
	memcpy(&incomingSequence, packet, sizeof(incomingSequence));
	incomingSequence = endianFromLittle(incomingSequence);
	packet += sizeof(incomingSequence);
	npacket -= sizeof(incomingSequence);

	// first packet? reset incoming sequence
	if (s->incomingSequence == 0)
	{
		s->incomingSequence = incomingSequence;
	}

	// notify opus of missing audio packets
	for (uint32_t ii = s->incomingSequence; ii < incomingSequence; ++ii)
	{
		const int nsamples = opus_decode_float(s->decoder, nullptr, 0, e->monoBuffer, Engine::c_nmonoBuffer, 0);
		if (nsamples > 0)
		{
			const uint32_t outputSamples = s->outputResampler.outputSamples(nsamples);
			const size_t existingSize = s->incomingData.size();
			s->incomingData.reserve(existingSize + outputSamples);
			s->outputResampler.resampleMono(e->monoBuffer, nsamples, s->incomingData.data() + existingSize, outputSamples);
		}
	}

	// process incoming data
	while (npacket)
	{
		if (npacket < 3)
		{
			break;
		}

		uint16_t audioPacketSize;
		memcpy(&audioPacketSize, packet, sizeof(audioPacketSize));
		audioPacketSize = endianFromLittle(audioPacketSize);
		packet += sizeof(audioPacketSize);
		npacket -= sizeof(audioPacketSize);

		if (incomingSequence >= s->incomingSequence)
		{
			const int nsamples = opus_decode_float(s->decoder, packet, audioPacketSize, e->monoBuffer, Engine::c_nmonoBuffer, 0);
			if (nsamples > 0)
			{
				const uint32_t outputSamples = s->outputResampler.outputSamples(nsamples);
				const size_t existingSize = s->incomingData.size();
				s->incomingData.resize(existingSize + outputSamples);
				s->outputResampler.resampleMono(e->monoBuffer, nsamples, s->incomingData.data() + existingSize, outputSamples);
			}
		}
		
		++incomingSequence;
		packet += audioPacketSize;
		npacket -= audioPacketSize;
	}
}

uint32_t voice::engineGetSourceAudio(Engine* /*e*/, Source* s, const float** monoSamples)
{
	*monoSamples = s->incomingData.data();
	return static_cast<uint32_t>(s->incomingData.size());
}

void voice::engineConsumeSourceAudio(Engine* /*e*/, Source* s, uint32_t samples)
{
	s->incomingData.erase(s->incomingData.begin(), s->incomingData.begin() + samples);
}
