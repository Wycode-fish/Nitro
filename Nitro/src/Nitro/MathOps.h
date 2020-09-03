#pragma once
#include "Core.h"
#include "Rttr.h"
#include "glm/glm.hpp"

namespace Nitro
{
	constexpr unsigned int CompileCheckLog2Floor(const u64& X)
	{
		return 1 + (X == 1) ? 0 : CompileCheckLog2Floor(X >> 1);
	}

	template <typename T, size_t SIZE>
	size_t ArraySize(T(&)[SIZE])
	{
		return SIZE;
	}

	template<typename T, typename EnableIf<sizeof(T) == 32, u32>::type* = nullptr>
	size_t OnBitCount(T x)
	{
		static const u32 masks[5]{
			0x55555555,/*...0101*/
			0x33333333,/*...0011*/
			0x0F0F0F0F,/*00001111....*/
			0X00FF00FF,
			0x0000FFFF
		};
		x = x & masks[0] + (x >> 1) & masks[0];
		x = x & masks[1] + (x >> 2) & masks[1];
		x = x & masks[2] + (x >> 4) & masks[2];
		x = x & masks[3] + (x >> 8) & masks[3];
		x = x & masks[4] + (x >> 16) & masks[4];
		return x;
	}
	template<typename T, typename EnableIf<sizeof(T) == 64, u64>::type* = nullptr>
	size_t OnBitCount(T x)
	{
		static const u64 masks[6]{
			0x5555555555555555,/*...0101*/
			0x3333333333333333,/*...0011*/
			0x0F0F0F0F0F0F0F0F,/*00001111....*/
			0X00FF00FF00FF00FF,
			0x0000FFFF0000FFFF,
			0x00000000FFFFFFFF
		};
		x = x & masks[0] + (x >> 1) & masks[0];
		x = x & masks[1] + (x >> 2) & masks[1];
		x = x & masks[2] + (x >> 4) & masks[2];
		x = x & masks[3] + (x >> 8) & masks[3];
		x = x & masks[4] + (x >> 16) & masks[4];
		x = x & masks[5] + (x >> 32) & masks[5];
		return x;
	}
}