#pragma once

#include "UtilConfig.h"

#define HASH_STATES_1(S)		EXPAND(Nitro::Util::hash_state(S))
#define HASH_STATES_2(S, ...)	EXPAND(Nitro::Util::hash_state(S, 1, HASH_STATES_1(__VA_ARGS__)))
#define HASH_STATES_3(S, ...)	EXPAND(Nitro::Util::hash_state(S, 1, HASH_STATES_2(__VA_ARGS__)))
#define HASH_STATES_4(S, ...)	EXPAND(Nitro::Util::hash_state(S, 1, HASH_STATES_3(__VA_ARGS__)))
#define HASH_STATES_5(S, ...)	EXPAND(Nitro::Util::hash_state(S, 1, HASH_STATES_4(__VA_ARGS__)))
#define HASH_STATES_I(N, ...)	EXPAND(HASH_STATES_##N(__VA_ARGS__))
#define HASH_STATES(N, ...) HASH_STATES_I(N, __VA_ARGS__)

namespace Nitro
{
	namespace Util
	{
		// @ ------------------------------------------------------------
		// @						Hash Ops
		// @ ------------------------------------------------------------
		extern hashkey_t hash_range(const u32* start, const u32* end, hashkey_t original);

		template <typename T>
		inline hashkey_t hash_state(const T* state, size_t count = 1, hashkey_t original = 2166136261U)
		{
			// @ Fun fact:
			// @		sizeof(T) & 3 == 0
			// @		is actually equals to
			// @		sizeof(T) & (3 == 0)
			// @		* the precedence of '==' is over '&'
			static_assert(((sizeof(T) & 3) == 0) && (alignof(T) >= 4), "state obj isn't 32-bit aligned.");
			return hash_range((u32*)state, (u32*)(state + count), original);
		}

		extern hashkey_t hash_string(const std::string& str);
	}
}