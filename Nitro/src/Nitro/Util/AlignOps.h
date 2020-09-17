#pragma once

namespace Nitro
{
	namespace Util
	{
		// @ ------------------------------------------------------------
			// @					Alignment Calculation
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
	}
}