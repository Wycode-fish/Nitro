#include "NtPCH.h"
#include "Timer.h"

namespace Nitro
{
	namespace Util
	{
		double Timer::sm_MsNow = 0l;

		double Timer::GetDelta_HighPrecision()
		{
#ifdef NT_PLATFORM_WINDOWS
			LARGE_INTEGER freq;
			LARGE_INTEGER curr;
			QueryPerformanceFrequency(&freq);
			QueryPerformanceCounter(&curr);
			double delta = (curr.QuadPart - Timer::sm_MsNow) / (freq.QuadPart / 1000.0f);
			sm_MsNow = static_cast<double>(curr.QuadPart);
#else
			timeval curr;
			gettimeofday(&curr, NULL);
			double msCurr = curr.tv_sec * 1000.0f + curr.tv_usec / 1000.0f;
			double delta = msCurr - sm_MsNow;
			sm_MsNow = msCurr;
#endif
			return delta;
		}
	}
}
