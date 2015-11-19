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

#include <vector>
#include <time.h>
#include <tiny/audio/render.h>
#include <tiny/audio/capture.h>
#include <tiny/audio/resample.h>
#include <tiny/platform.h>

using namespace tiny;
using namespace tiny::audio;

int main()
{
	if (!platformStartup())
		return -1;

	ICaptureDevice* mic = acquireDefaultCaptureDevice();
	if (!mic)
		return -1;

	IRenderDevice* speaker = acquireDefaultRenderDevice(44100);
	if (!speaker)
		return -1;

	typedef resample::Linear Resampler;
	Resampler resampler(mic->sampleRate(), speaker->sampleRate());
	const int nmicsamples = mic->samplesPer16ms();

	const float* micSamples = nullptr;
	int remainingMicSamples = 0;

	if (!speaker->start() || !mic->start())
	{
		return -1;
	}

	const time_t end = time(nullptr)+10;
	while (time(nullptr) < end)
	{		
		float* outBuffer;
		int samplesOut = speaker->acquireBuffer(&outBuffer);
		if (samplesOut < 0)
		{
			break;
		}

		while (samplesOut)
		{
			// acquire mic samples if we're out
			if (remainingMicSamples == 0)
			{
				micSamples = mic->get16msOfSamples();
				if (micSamples)
				{
					remainingMicSamples = nmicsamples;
				}
			}

			if (remainingMicSamples)
			{
				// write as many samples as possible
				int samplesToWrite = samplesOut;
				int samplesToRead = resampler.inputSamples(samplesToWrite);
				if (samplesToRead > remainingMicSamples)
				{
					samplesToRead = remainingMicSamples;
					samplesToWrite = resampler.outputSamples(samplesToRead);
				}

				if (mic->channels() == 1)
				{
					std::vector<float> buffer(samplesToWrite);
					resampler.resampleMonoToStereo(micSamples, samplesToRead, outBuffer, samplesToWrite);
				}
				else
				{
					resampler.resampleStereo(micSamples, samplesToRead, outBuffer, samplesToWrite);
				}

				// adjust buffers
				micSamples += samplesToRead;
				remainingMicSamples -= samplesToRead;
				outBuffer += 2*samplesToWrite;
				samplesOut -= samplesToWrite;
			}
		}

		speaker->commitBuffer();
	}

	speaker->release();
	mic->release();
}
