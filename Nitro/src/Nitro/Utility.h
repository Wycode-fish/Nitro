#pragma once
#include <string>
#include "Rttr.h"

#ifdef _M_X64
#define __enable_sse_crc32__ 1
#else
#define __enable_sse_crc32__ 0
#endif

// @ use crc function as hash func
#if __enable_sse_crc32__
#pragma intrinsic(_mm_crc32_u32)
#pragma intrinsic(_mm_crc32_u64)
#endif
typedef unsigned long long u64;
typedef unsigned int u32;
typedef size_t hashkey_t;

#define EXPAND(x) x
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
		// @					String Manipulation
		// @ ------------------------------------------------------------
		extern std::wstring		stdstring_to_stdwstring(const std::string&);

		// @ ------------------------------------------------------------
		// @						File Ops
		// @ ------------------------------------------------------------
		extern bool				is_file_accessible(const std::string&);
		extern std::string		read_file(const std::string& path);
		extern std::string		read_file2(const std::string& path);
		extern std::string		file_path2name(const std::string& path, bool withExt = false);
		// @ ------------------------------------------------------------
		// @					Variable Conversion
		// @ ------------------------------------------------------------
		template <typename T>
		std::string integral_to_hexstr(T w, size_t hex_len = sizeof(T) << 1) {
			static const char* digits = "0123456789ABCDEF";
			std::string rc(hex_len, '0');
			for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
				rc[i] = digits[(w >> j) & 0x0f];
			return rc;
		}
		// @ ------------------------------------------------------------
		// @					Memory Ops
		// @ ------------------------------------------------------------
		template <typename T>
		__forceinline T alignUp(T value, size_t alignment)
		{
			size_t mask = alignment - 1;
			// here we +(alignment - 1) instead of +(alignment),
			// so that if value is already aligned, it won't get
			// shifted up by 1 unit for no reason
			return (T)(((size_t)value + mask) & ~mask);
		}
		template <typename T>
		__forceinline T alignDown(T value, size_t alignment)
		{
			size_t mask = alignment - 1;
			return (T)((size_t)value & ~mask);
		}

		template <typename T>
		__forceinline bool isAligned(T value, size_t alignment)
		{
			return ((size_t)value & (alignment - 1)) == 0;
		}

		template <typename T>
		__forceinline T divide_1base(T op1, size_t op2)
		{
			return (T)((op1 + op2 - 1) / op2);
		}

		// @ __restrict: (compiler keyword)
		// @	is basically a promise to the compiler that 
		// @	for the scope of the pointer, 
		// @	the target of the pointer will only be 
		// @	accessed through that pointer (and pointers copied from it)
		extern void simd_memcpy(void* __restrict dest, const void* __restrict src, size_t sizeInDQWord);

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

		// @ ------------------------------------------------------------
		// @					Sort Ops
		// @ ------------------------------------------------------------
		template <typename _ElementType, u32 _ArraySize, NT_TEMPLATE_CONDITION((_ArraySize& (_ArraySize - 1)) == 0)>
		void bitonic_sort_cpu(_ElementType arr[], RSG::ntDelegate<bool(_ElementType, _ElementType)> compareFunc)
		{
			for (u32 groupSize = 2; groupSize <= _ArraySize; groupSize <<= 1)
			{
				for (u32 gid = 0; gid < _ArraySize / groupSize; ++gid)
				{
					u32 opCode = BIT(0) & gid;
					u32 currentInnerGroupSize = groupSize;

					for (u32 currentInnerGroupSize = groupSize; currentInnerGroupSize > 1; currentInnerGroupSize>>=1)
					{
						u32 currentInnerGroupCount = groupSize / currentInnerGroupSize;
						u32 compareDistance = currentInnerGroupSize >> 1;

						for (u32 currentInnerGroupIndex = 0; currentInnerGroupIndex < currentInnerGroupCount; ++currentInnerGroupIndex)
						{
							for (u32 eid = 0; eid < compareDistance; ++eid)
							{
								u32 left = gid * groupSize + currentInnerGroupIndex * currentInnerGroupSize + eid;
								_ElementType& l = arr[left];
								_ElementType& r = arr[left + compareDistance];

								bool comparePass = compareFunc(l, r);
								if ((comparePass && opCode == 0) || (!comparePass && opCode == 1))
								{
									std::swap(l, r);
								}
							}
						}
					}
				}
			}
		}

		template <typename _ElementType, u32 _ArraySize, NT_TEMPLATE_CONDITION((_ArraySize& (_ArraySize - 1)) == 0)>
		void bitonic_sort_cpu2(_ElementType arr[], RSG::ntDelegate<bool(_ElementType, _ElementType)> compareFunc)
		{
			for (u32 groupSize = 2, groupCount = _ArraySize >> 1; groupSize <= _ArraySize; groupSize<<=1, groupCount>>=1)
			{
				for (u32 gid = 0; gid < groupCount; ++gid)
				{
					bitonic_sort_cpu2_helper(arr, groupSize, gid * groupSize, gid & BIT(0), compareFunc);
				}
			}
		}

		template <typename _ElementType>
		void bitonic_sort_cpu2_helper(_ElementType arr[], u32 groupSize, u32 elementIndex, u32 opCode, RSG::ntDelegate<bool(_ElementType, _ElementType)> compareFunc)
		{
			if (groupSize == 1)
			{
				return;
			}
			u32 compareDistance = groupSize >> 1;
			for (u32 i = elementIndex; i < elementIndex + compareDistance; ++i)
			{
				bool comparePass = compareFunc(arr[i], arr[i + compareDistance]);
				if ((opCode == 0 && comparePass) || (opCode == 1 && !comparePass))
				{
					std::swap(arr[i], arr[i + compareDistance]);
				}
			}
			bitonic_sort_cpu2_helper(arr, compareDistance, elementIndex, opCode, compareFunc);
			bitonic_sort_cpu2_helper(arr, compareDistance, elementIndex + compareDistance, opCode, compareFunc);
		}
	}
}