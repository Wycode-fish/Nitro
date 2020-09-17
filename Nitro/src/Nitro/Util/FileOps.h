#pragma once

#include "UtilConfig.h"

namespace Nitro
{
	namespace Util
	{
		// @ ------------------------------------------------------------
		// @						File Ops
		// @ ------------------------------------------------------------
		extern bool				is_file_accessible(const std::string&);
		extern std::string		read_file(const std::string& path);
		extern std::string		read_file2(const std::string& path);
		extern std::string		file_path2name(const std::string& path, bool withExt = false);
	}
}