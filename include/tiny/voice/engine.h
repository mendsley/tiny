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

#ifndef TINY_VOICE__ENGINE_H
#define TINY_VOICE__ENGINE_H

#include <stdint.h>

namespace tiny
{
	namespace audio { class ICaptureDevice; }

	namespace voice
	{
		struct Engine;
		class Source;

		Engine* engineCreate(audio::ICaptureDevice* microphone, uint32_t sampleRate = 48000);
		void engineRelease(Engine* e);

		void engineAddSource(Engine* e, Source* s);
		void engineRemoveSource(Engine* e, Source* s);

		uint32_t engineGeneratePacket(Engine* e, uint8_t* packet, uint32_t npacket);
		void engineProcessPacket(Engine* e, Source* s, const uint8_t* packet, uint32_t npacket);

		uint32_t engineGetSourceAudio(Engine* e, Source* s, const float** monoSamples);
		void engineConsumeSourceAudio(Engine* e, Source* s, uint32_t samples);
	}
}

#endif // TINY_VOICE__ENGINE_H
