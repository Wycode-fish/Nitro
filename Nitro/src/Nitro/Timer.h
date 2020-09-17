#pragma once

namespace Nitro
{
	namespace Util
	{
		class Timer
		{
		public:
			static double GetDelta_HighPrecision();
		private:
			static double sm_MsNow;
		};
	}
}
