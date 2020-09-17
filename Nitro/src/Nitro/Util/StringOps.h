#pragma once

#include "UtilConfig.h"

// @ Using vendor code fmtlib here
// @ e.g. 
// @	std::string s = fmt::format("I'd rather be {1} than {0}.", "right", "happy");
#define NT_STDSTR_FORMAT(...) fmt::format(__VA_ARGS__)
// @ std::string => wchar_t*
#define NT_STDSTR_TO_STDWSTR(str) Nitro::Util::stdstring_to_stdwstring(str)
#define NT_STDWSTR_FORMAT(...) NT_STDSTR_TO_STDWSTR(NT_STDSTR_FORMAT(__VA_ARGS__))

namespace Nitro
{
	namespace Util
	{
		// @ ------------------------------------------------------------
		// @					String Manipulation
		// @ ------------------------------------------------------------
		extern std::wstring	stdstring_to_stdwstring(const std::string&);


	}
}