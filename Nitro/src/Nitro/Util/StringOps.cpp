#include "NtPCH.h"

namespace Nitro
{
	namespace Util
	{
		std::wstring stdstring_to_stdwstring(const std::string& str)
		{
			std::wstring wstr(str.begin(), str.end());
			return wstr;
		}
	}
}