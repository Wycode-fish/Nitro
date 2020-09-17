#include "NtPCH.h"
#include "SimdOps.h"
#include "UtilConfig.h"
#include "AlignOps.h"

namespace Nitro
{
	namespace Util
	{
		void simd_memcpy(void* __restrict dest, const void* __restrict src, size_t numInQWord)
		{
			// @ align to 16 is due to __alignment_memblk is 128-bit-aligned. (16 bytes = 128 bits)
			UASSERT(isAligned(dest, 16));
			UASSERT(isAligned(src, 16));

			__m128i* __restrict pDest = (__m128i * __restrict)dest;
			const __m128i* __restrict pSrc = (const __m128i * __restrict)src;

			// @ to find how many quadwords precede a cache line boundary(64-bytes).
			// @ we need to copy them separately if the mem region cross a cache line boundary.
			// @
			// @ Note here *WORD*= 4-byte, *QWORD*=16-byte, hence *CacheLineSize*=64-bytes=4-QWORD
			// @ step-1 : ((size_t)pSrc >> 4)		
			// @				=> pSrc / 16, src's address value in *QWORD*
			// @ step-2 : Res(step-1) & 3			
			// @				=> remainder of Res(step-1) / 4, which is, 
			// @					number of remained *QWORD* when try to measure pSrc in *CacheLineSize*
			// @ step-3 : 4 - Res(step-2)
			// @				=> number of extra *QWORD* pSrc address needs to make it align
			// @					to *CacheLineSize*
			size_t mem_size_front_m64bytes = (4 - ((size_t)pSrc >> 4) & 3) & 3;
			if (mem_size_front_m64bytes > numInQWord)
			{
				mem_size_front_m64bytes = numInQWord;
			}
			switch (mem_size_front_m64bytes)
			{
			case 3: _mm_stream_si128(pDest + 2, _mm_load_si128(pSrc + 2));
			case 2: _mm_stream_si128(pDest + 1, _mm_load_si128(pSrc + 1));
			case 1: _mm_stream_si128(pDest + 0, _mm_load_si128(pSrc + 0));
			default: break;
			}

			if (mem_size_front_m64bytes == numInQWord)
			{
				return;
			}

			pDest += mem_size_front_m64bytes;
			pSrc += mem_size_front_m64bytes;
			numInQWord -= mem_size_front_m64bytes;

			// @ do cacheline fetch if size to copy is large enough,
			// @ this is a pre-step before do the actual stream copy
			// @ make simd faster
			size_t cachelines = numInQWord >> 2;
			switch (cachelines)
			{
			default:	// @ put default ahead, to handle case when cache line number larger than pre-defined prefetch below
			case 10: _mm_prefetch((char*)(pSrc + 36), _MM_HINT_NTA);
			case 9: _mm_prefetch((char*)(pSrc + 32), _MM_HINT_NTA);
			case 8: _mm_prefetch((char*)(pSrc + 28), _MM_HINT_NTA);
			case 7: _mm_prefetch((char*)(pSrc + 24), _MM_HINT_NTA);
			case 6: _mm_prefetch((char*)(pSrc + 20), _MM_HINT_NTA);
			case 5: _mm_prefetch((char*)(pSrc + 16), _MM_HINT_NTA);
			case 4: _mm_prefetch((char*)(pSrc + 12), _MM_HINT_NTA);
			case 3: _mm_prefetch((char*)(pSrc + 8), _MM_HINT_NTA);
			case 2: _mm_prefetch((char*)(pSrc + 4), _MM_HINT_NTA);
			case 1: _mm_prefetch((char*)(pSrc + 0), _MM_HINT_NTA);
				// @ prefetch to cache backward, stream load forward.
				for (size_t i = cachelines; i > 0; --i)
				{
					if (i >= 10)
					{
						_mm_prefetch((char*)(pSrc + 40), _MM_HINT_NTA);
					}
					// @ do 4 dqword copies at once to reduce stall
					_mm_stream_si128(pDest, _mm_load_si128(pSrc));
					_mm_stream_si128(pDest + 1, _mm_load_si128(pSrc + 1));
					_mm_stream_si128(pDest + 2, _mm_load_si128(pSrc + 2));
					_mm_stream_si128(pDest + 3, _mm_load_si128(pSrc + 3));

					pDest += 4;
					pSrc += 4;
				}
			case 0:
				break;
			}

			// @ dqwords left over 64-byte cache line sizes
			switch (numInQWord & 3)
			{
			case 3: _mm_stream_si128(pDest + 2, _mm_load_si128(pSrc + 2));
			case 2: _mm_stream_si128(pDest + 1, _mm_load_si128(pSrc + 1));
			case 1: _mm_stream_si128(pDest + 0, _mm_load_si128(pSrc + 0));
			default: break;
			}
			// ??
			_mm_sfence();
		}
	}
}
