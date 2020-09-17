#include "NtPCH.h"
#include "HashOps.h"
#include "AlignOps.h"
#include "SimdOps.h"

namespace Nitro
{
	namespace Util
	{
		hashkey_t hash_range(const u32* start, const u32* end, hashkey_t original)
		{
#if __enable_sse_crc32__
			const u64* ptr64 = (const u64*)alignUp(start, 8);
			const u64* end64 = (const u64*)alignDown(end, 8);
			if (ptr64 > (u64*)start)
			{
				original = _mm_crc32_u32(original, *start);
			}
			while (ptr64 < end64)
			{
				original = _mm_crc32_u64(original, *ptr64++);
			}
			if (ptr64 < (u64*)end)
			{
				original = _mm_crc32_u32(original, *(u32*)ptr64);
			}
#else
			for (u32* ptr = const_cast<u32*>(start); ptr < end; ++ptr)
			{
				original = 16777619U * original ^ *ptr;
			}
#endif
			return original;
		}

		hashkey_t hash_string(const std::string& str)
		{
			static std::hash<std::string> hasher;
			return hasher(str);
		}
	}
}