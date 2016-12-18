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

#ifndef TINY_HASH__FNV_H
#define TINY_HASH__FNV_H

#include <stdint.h>

namespace tiny
{
	namespace detail
	{
		template<typename T> struct consts;

		template<> struct consts<uint32_t>
		{
			static constexpr uint32_t offset = 2166136261;
			static constexpr uint32_t prime = 16777619; // 2^24 + 2^8 + 0x93
		};

		template<> struct consts<uint64_t>
		{
			static constexpr uint64_t offset = 14695981039346656037ull;
			static constexpr uint64_t prime = 1099511628211ull;
		};

		// fnv-1a
		template<typename T>
		static inline T fnv1a(const void* ptr, size_t n)
		{
			const uint8_t* p = static_cast<const uint8_t*>(ptr);
			T v = consts<T>::offset;
			for (size_t ii = 0; ii < n; ++ii, ++p)
			{
				v = (v ^ *p) * consts<T>::prime;
			}

			return v;
		}

		template<typename T>
		static inline constexpr T fnv1a_compiletime(const char* ptr, size_t n, T existing = consts<T>::offset)
		{
			return n ? fnv1a_compiletime(ptr + 1, n - 1, static_cast<uint32_t>(1ULL * (ptr[0] ^ existing) * consts<T>::prime)) : existing;
		}
	}

	static constexpr auto fnv1a = detail::fnv1a<uint32_t>;
	static constexpr auto fnv1a_64 = detail::fnv1a<uint64_t>;

	template<uint32_t size>
	static inline uint32_t fnv1a_cstr(const char (&str)[size])
	{
		return fnv1a(str, size - 1);
	}
}

constexpr uint32_t operator"" _fnv1a(const char* s, size_t n)
{
	return tiny::detail::fnv1a_compiletime<uint32_t>(s, n);
}

constexpr uint64_t operator"" _fnv1a_64(const char* s, size_t n)
{
	return tiny::detail::fnv1a_compiletime<uint64_t>(s, n);
}

#endif // TINY_HASH__FNV_H
