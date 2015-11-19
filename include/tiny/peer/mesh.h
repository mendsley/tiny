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

#ifndef TINY_PEER__MESH_H
#define TINY_PEER__MESH_H

#include <stdint.h>

namespace tiny
{
	namespace peer
	{
		struct Message;

		struct MeshState
		{
			enum E
			{
				Created,
				Starting,
				// same as Running, but only on initial edge trigger of state.
				StartComplete,
				Running,
				Invalid,
			};
		};

		struct PeerState
		{
			enum E
			{
				Negotiating,
				Connected,
				Invalid,
			};
		};

		static const uint32_t InvalidMeshPeer = 0xFFFFFFFF;

		// opaque peer-to-peer mesh
		struct Mesh;

		// create a new peer-to-peer mesh that supports up to `maxPeers'
		// remote connections and runs on the port `port'. If the platform
		// supports it, setting `port' to 0 allows the platform to operate
		// on an arbitrary port.
		Mesh* meshCreate(uint32_t maxPeers, uint64_t localId, uint16_t port);

		// destroys an existing peer mesh
		void meshDestroy(Mesh* m);

		// update peer mesh state. dispatches data to peers that may be retreived
		// with `meshReceive'
		MeshState::E meshUpdate(Mesh* m);

		// starts processing a session with the supplied session key. If
		// `stunHost' is not `nullptr' then the mesh will use
		// stun:`stunHost':`stunPort' to obtain server reflexive candidates
		// to aid in NAT traversal.
		bool meshStartSession(Mesh* m, const char* stunHost, uint16_t stunPort);

		// sets the session key for the peer-to-peer session. any client
		// connecting to this mesh will need to have the same key set
		// before `meshConnectToPeer' is called. key distribution must
		// use a separate signaling mechanism; tinypeer does not provide
		// this signaling. this function may be called at any time before
		// the local peer attempts to connect to remote peers, and before
		// remote peers attempt to connect
		void meshSetSessionKey(Mesh* m, const uint8_t* key, int nkey);

		// end a running peer-to-peer session
		void meshEndSession(Mesh* m);

		// acquires the number of bytes needed to serialize the local
		// address of the peer mesh. this address is platform specific
		// and may map to multiple host or reflexive addresses. this
		// function will return 0 until `meshUpdate' returns a state
		// of `MeshState::StartComplete' or `MeshState::Running'
		uint32_t meshLocalAddressSize(Mesh* m);

		// serialize the local address of the peer mesh to a binary stream.
		// this will write exactly `meshLocalAddressSize` to the pointer
		// `out'
		void meshSerializeLocalAddress(Mesh* m, uint8_t* out);

		// start the process of connecting to a peer. returns an index
		// to the connection
		uint32_t meshConnectToPeer(Mesh* m, uint64_t remoteId
			, const uint8_t* remoteAddress, uint32_t nremoteAddress);

		// disconnect from a peer. does not signal the peer, simply
		// stops processing incoming packets
		void meshDisconnectPeer(Mesh* m, uint32_t peer, uint64_t id);

		// get the connection status of a peer
		PeerState::E meshPeerState(Mesh* m, uint32_t peer);

		// send unreliable data to a connection
		void meshSendUnreliableDataToPeer(Mesh* m, uint32_t peer
			, const void* p, uint32_t n);

		// receive data from a peer connection. fills `messages', and
		// `nmessages' and returns `true' if data is available. otherwise returns
		// `false'. the mesh itself will release all messages internally on the next
		// update.
		bool meshReceive(Mesh* m, uint32_t peer
			, Message*** messages, uint32_t* nmessages);
	}
}

#endif // TINY_PEER__MESH_H
