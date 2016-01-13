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

#if TINY_PLATFORM_WINDOWS

#include <stdint.h>
#include <vector>
#include "tiny/debug/dbg.h"
#include "tiny/endian.h"
#include "tiny/net2/af.h"
#include "tiny/net2/transport.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>

using namespace tiny;
using namespace tiny::net2;

// enumerate adapters matching a platform address family
static int enumerateAdapters(int platformAF, uint16_t bePort, PlatformAddress* addresses, int naddresses)
{
	// enumerate local adapters
	static const ULONG flags = 0
		| GAA_FLAG_SKIP_ANYCAST
		| GAA_FLAG_SKIP_MULTICAST
		| GAA_FLAG_SKIP_DNS_SERVER
		| GAA_FLAG_SKIP_FRIENDLY_NAME
		;

	ULONG bufferSize = 0;
	if (ERROR_BUFFER_OVERFLOW != GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, nullptr, &bufferSize))
	{
		return 0;
	}

	std::vector<uint8_t> storage(bufferSize);
	IP_ADAPTER_ADDRESSES* adapters = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(storage.data());
	if (ERROR_SUCCESS != GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, adapters, &bufferSize))
	{
		return 0;
	}

	uint32_t numAdpaters = 0;
	for (IP_ADAPTER_ADDRESSES* adapter = adapters; adapter; adapter = adapter->Next)
	{
		if (adapter->OperStatus == IfOperStatusUp)
		{
			for (IP_ADAPTER_UNICAST_ADDRESS* address = adapter->FirstUnicastAddress; address; address = address->Next)
			{
				const sockaddr* sockaddr = address->Address.lpSockaddr;
				if (sockaddr->sa_family == platformAF && address->Address.iSockaddrLength <= sizeof(addresses->storage))
				{
					++numAdpaters;
					if (naddresses)
					{
						addresses->size = address->Address.iSockaddrLength;
						memcpy(addresses->storage, address->Address.lpSockaddr, addresses->size);
					
						switch (platformAF)
						{
						case AF_INET:
							reinterpret_cast<sockaddr_in*>(addresses->storage)->sin_port = bePort;
							break;

						case AF_INET6:
							reinterpret_cast<sockaddr_in6*>(addresses->storage)->sin6_port = bePort;
							break;

						default:
							debug()->assertf(false, "Unknown platform address family %d", platformAF);
							break;
						}

						++addresses;
						--naddresses;
					}
				}
			}
		}
	}

	return numAdpaters;
}

namespace tiny
{
	namespace net2
	{
		namespace udp
		{
			template<int PlatformAF>
			class TransportProvider : public ITransportProvider
			{
			public:
				TransportProvider(uint16_t bePort);
				~TransportProvider();

				virtual void release();
				virtual int addressFamily();
				virtual int enumerateLocalAddresses(PlatformAddress* addresses, int naddresses);
				virtual ITransport* createTransport(const PlatformAddress& addr);

			private:
				TransportProvider(const TransportProvider&); // = delete
				TransportProvider& operator=(const TransportProvider&); // = delete

				const uint16_t bePort;
			};
		}
	}
}

ITransportProvider* createUdp4Transport(uint16_t port)
{
	return new udp::TransportProvider<AF_INET>(tiny::endianToBig(port));
}

ITransportProvider* createUdp6Transport(uint16_t port)
{
	return new udp::TransportProvider<AF_INET6>(tiny::endianToBig(port));
}

template<int PlatformAF>
udp::TransportProvider<PlatformAF>::TransportProvider(uint16_t bePort)
	: bePort(bePort)
{
}

template<int PlatformAF>
udp::TransportProvider<PlatformAF>::~TransportProvider()
{
}

template<int PlatformAF>
void udp::TransportProvider<PlatformAF>::release()
{
	delete this;
}

int udp::TransportProvider<AF_INET>::addressFamily()
{
	return AddressFamily::UDPv4;
}

int udp::TransportProvider<AF_INET6>::addressFamily()
{
	return AddressFamily::UDPv6;
}

template<int PlatformAF>
int udp::TransportProvider<PlatformAF>::enumerateLocalAddresses(PlatformAddress* addresses, int naddresses)
{
	return enumerateAdapters(PlatformAF, bePort, addresses, naddresses);
}

template<int PlatformAF>
ITransport* udp::TransportProvider<PlatformAF>::createTransport(const PlatformAddress& addr)
{
	return nullptr;
}

#endif // TINY_PLATFORM_WINDOWS
