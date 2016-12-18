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

#if TINY_PLATFORM_LINUX

#include <errno.h>
#include <fcntl.h>
#include <linux/random.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

using namespace tiny;
using namespace tiny::crypto;

#if defined(SYS_getrandom)
RandSource::RandSource()
{
}

RandSource::~RandSource()
{
}

bool RandSource::fill(uint8_t* p, uint32_t n)
{
	while (n > 0)
	{
		int written = syscall(SYS_getrandom, p, n, 0);
		if (written == -1)
		{
			if (errno == EINTR || errno == EAGAIN)
			{
				continue;
			}
			return false;
		}

		p += written;
		n -= written;
	}

	return true;
}

#else

RandSource::RandSource()
{
	int fd = open("/dev/urandom", O_RDONLY);
	platform = reinterpret_cast<void*>(static_cast<intptr_t>(fd));
}

RandSource::~RandSource()
{
	int fd = static_cast<int>(reinterpret_cast<uintptr_t>(platform));
	if (fd != -1)
	{
		close(fd);
	}
}

bool RandSource::fill(uint8_t* p, uint32_t n)
{
	int fd = static_cast<int>(reinterpret_cast<uintptr_t>(platform));
	if (fd == -1)
	{
		return false;
	}

	while (n > 0)
	{
		ssize_t result = read(fd, p, n);
		if (result == -1)
		{
			if (errno == EINTR || errno == EAGAIN)
			{
				continue;
			}
			return false;
		}

		p += result;
		n -= result;
	}

	return true;
}

#endif // defined(SYS_getrandom)

#endif // TINY_PLATFORM_LINUX
