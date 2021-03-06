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

#include "tiny/platform.h"
#include "tiny/crypto/rand.h"

#if TINY_PLATFORM_WINDOWS

#include <memory>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace tiny;
using namespace tiny::crypto;

static BOOLEAN APIENTRY RtlGenRandomFallback(void*, ULONG)
{
	return FALSE;
}

namespace
{
	// See RtlGenRandom: https://msdn.microsoft.com/en-us/library/windows/desktop/aa387694(v=vs.85).aspx
	typedef BOOLEAN (APIENTRY* RtlGenRandomFn)(void*, ULONG);
	static HMODULE g_advapi = nullptr;
	static RtlGenRandomFn g_RtlGenRandom = RtlGenRandomFallback;
}

RandSource::RandSource()
{
	if (!g_advapi)
	{
		g_advapi = LoadLibraryA("ADVAPI32.dll");
		if (!g_advapi)
		{
			// something is seriously wrong
			return;
		}

		auto fn = reinterpret_cast<RtlGenRandomFn>(GetProcAddress(g_advapi, "SystemFunction036"));
		if (fn)
		{
			g_RtlGenRandom = fn;
		}
	}
}

RandSource::~RandSource()
{
}

bool RandSource::fill(uint8_t* p, uint32_t n)
{
	return FALSE != g_RtlGenRandom(p, n);
}

#endif // TINY_PLATFORM_WINDOWS
