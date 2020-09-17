#pragma once

#include "UtilConfig.h"

#include <emmintrin.h>
#include <intrin.h>


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

namespace Nitro
{
	namespace Util
	{
		// @ __restrict: (compiler keyword)
		// @	is basically a promise to the compiler that 
		// @	for the scope of the pointer, 
		// @	the target of the pointer will only be 
		// @	accessed through that pointer (and pointers copied from it)
		extern void simd_memcpy(void* __restrict dest, const void* __restrict src, size_t sizeInDQWord);
	}
}