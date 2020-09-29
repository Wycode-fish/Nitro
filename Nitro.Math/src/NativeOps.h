#pragma once

#include "NitroConfig_Math.h"

#pragma region SIMD Dictionary
/*
ps => packed single precision (4 32-bit float)
sd => scalar double precision (1 64-bit float at the least-significant position)

Nameing Convention: _mm_<SIMD_OP>_<DATA_TYPE>

1. _mm_castps_si128 => cast packed 4 32-bit float to packed 4 32-bit integer
2. _mm_shuffle_ps => work with macro _MM_SHUFFLE to change order of 4 floats stored in a 128-bit register
    https://stackoverflow.com/questions/13153584/mm-shuffle-ps-equivalent-for-integer-vectors-m128i
3. _mm_srli_si128 / _mm_srli_epi32 => shift whole or shift each 32-bit
4. _mm_andnot_ps => operands order matters! op1 & (~op2)
*/
#pragma endregion

namespace Nitro
{
	namespace Math
	{
#ifdef NT_WINDOWED_APP
		INLINE M128 SplatZero()
		{
			return XMVectorZero();
		}

    #if !defined(_XM_NO_INTRINSICS_) && defined(_XM_SSE_INTRINSICS_)
            // @ IEEE single/double precision number representation:
            // @    - https://docs.microsoft.com/en-us/cpp/build/ieee-floating-point-representation?view=vs-2019
            // @    
            // @    1 = 1 * 2^(0) = 0 x 0 011 1111 1 000 0000 0000 0000 0000 0000 = 0x3F80000
            // @                    0 x S XXX XXXX X MMM MMMM MMMM MMMM MMMM MMMM
            // @    where 'S' => Sign bit
            // @          'X' => Exponent bits
            // @          'M' => Mantissa bits
            INLINE M128 SplatOne(M128 zero = SplatZero())
            {
                __m128i AllBits = _mm_castps_si128(_mm_cmpeq_ps(zero, zero));
                return _mm_castsi128_ps(_mm_slli_epi32(_mm_srli_epi32(AllBits, 25), 23));    // return 0x3F800000
                //return _mm_cvtepi32_ps(_mm_srli_epi32(SetAllBits(zero), 31));                // return (float)1;  (alternate method)
            }
    
        #if defined(_XM_SSE4_INTRINSICS_)
                INLINE XMVECTOR CreateXUnitVector(XMVECTOR one = SplatOne())
                {
                    return _mm_insert_ps(one, one, 0x0E);
                }
                INLINE XMVECTOR CreateYUnitVector(XMVECTOR one = SplatOne())
                {
                    return _mm_insert_ps(one, one, 0x0D);
                }
                INLINE XMVECTOR CreateZUnitVector(XMVECTOR one = SplatOne())
                {
                    return _mm_insert_ps(one, one, 0x0B);
                }
                INLINE XMVECTOR CreateWUnitVector(XMVECTOR one = SplatOne())
                {
                    return _mm_insert_ps(one, one, 0x07);
                }
                INLINE XMVECTOR SetWToZero(FXMVECTOR vec)
                {
                    return _mm_insert_ps(vec, vec, 0x08);
                }
                INLINE XMVECTOR SetWToOne(FXMVECTOR vec)
                {
                    return _mm_blend_ps(vec, SplatOne(), 0x8);
                }
        #else
                INLINE M128 CreateXUnitVector(M128 one = SplatOne())
                {
                    return _mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(one), 12));
                }
                INLINE M128 CreateYUnitVector(M128 one = SplatOne())
                {
                    M128 unitx = CreateXUnitVector(one);
                    return _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(unitx), 4));
                }
                INLINE M128 CreateZUnitVector(M128 one = SplatOne())
                {
                    M128 unitx = CreateXUnitVector(one);
                    return _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(unitx), 8));
                }
                INLINE M128 CreateWUnitVector(M128 one = SplatOne())
                {
                    return _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(one), 12));
                }
                INLINE M128 SetWToZero(const M128 vec)
                {
                    __m128i MaskOffW = _mm_srli_si128(_mm_castps_si128(_mm_cmpeq_ps(vec, vec)), 4);
                    return _mm_and_ps(vec, _mm_castsi128_ps(MaskOffW));
                }
                INLINE M128 SetWToOne(const M128 vec)
                {
                    return _mm_movelh_ps(vec, _mm_unpackhi_ps(vec, SplatOne()));
                }
        #endif
    
    #else // !_XM_SSE_INTRINSICS_
    
            INLINE XMVECTOR SplatOne() { return XMVectorSplatOne(); }
            INLINE XMVECTOR CreateXUnitVector() { return g_XMIdentityR0; }
            INLINE XMVECTOR CreateYUnitVector() { return g_XMIdentityR1; }
            INLINE XMVECTOR CreateZUnitVector() { return g_XMIdentityR2; }
            INLINE XMVECTOR CreateWUnitVector() { return g_XMIdentityR3; }
            INLINE XMVECTOR SetWToZero(FXMVECTOR vec) { return XMVectorAndInt(vec, g_XMMask3); }
            INLINE XMVECTOR SetWToOne(FXMVECTOR vec) { return XMVectorSelect(g_XMIdentityR3, vec, g_XMMask3); }
    
    #endif


#endif
	}
}