/**
 * Copyright 2011-2016 Matthew Endsley
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

#pragma once

#include <stdint.h>

namespace tiny
{
	namespace net2
	{
		struct PlatformAddress
		{
			uint32_t size;
			uint8_t storage[28];
		};

		struct ConstBuffer
		{
#if defined(_MSC_VER)
			uint32_t len;
			const uint8_t* p;
#else
			const uint8_t* p;
			size_t len;
#endif
		};

		struct TransportRecvResult
		{
			enum E
			{
				Success,
				Failure,
				WouldBlock,
			};
		};

		// Manage communication across a single transport address
		class ITransport
		{
		public:
			// shutdown and release the transport
			virtual void release() = 0;

			// attempt to send a message to the specified remote address
			virtual bool send(const ConstBuffer* buffer, int nbuffers, const PlatformAddress& addr) = 0;
			virtual TransportRecvResult::E recv(void* buffer, int nbuffer, PlatformAddress* addr, int* read) = 0;

		protected:
			virtual ~ITransport() = 0;
		};

		// Core low-level networking interface.
		class ITransportProvider
		{
		public:
			
			// shutdown and release the transport interface
			virtual void release() = 0;

			// get the address family for the provider. only one transport
			// provider may exist for a given address family.
			virtual int addressFamily() = 0;

			// enumerate local addresses for the transport provider
			virtual int enumerateLocalAddresses(PlatformAddress* addresses, int naddresses) = 0;

			// create a transport object for the specified address
			virtual ITransport* createTransport(const PlatformAddress& addr) = 0;

		protected:
			virtual ~ITransportProvider() = 0;
		};
	}
}
