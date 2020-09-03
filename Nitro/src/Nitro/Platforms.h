#pragma once
#include "Core.h"
#include <vector>

namespace Nitro
{
	namespace Base
	{
		enum Platform : u8
		{
			Win64 = BIT(0),
			PS4 = BIT(1),
			XB1 = BIT(2),
			Count = 3
		};
		
		u8 ParseToPlatforms(const char* platform)
		{
			bool except = platform[0] == '!';
			const char* str = except ? platform + 1 : platform;
			u8 res = 0;
			if (str[0] == 'p' && str[1] == 'c')
				res = Platform::Win64;
			else if (str[0] == 'p' && str[1] == 's')
				res = Platform::PS4;
			else if (str[0] == 'x' && str[1] == 'b')
				res = Platform::XB1;

			NT_ASSERT(res, "invalid platform code presented.");

			res = !except ? res : ((u8)~0x0u >> (sizeof(Platform) - Platform::Count)) | ~res;
			return res;
		}
		std::string GetPlatformsString(const u8& platforms)
		{
			static const std::vector<const char*> map{"Win64", "PS4", "XB1"};
			std::string res;
			for (u8 i = 0; i < map.size(); ++i)
			{
				if (platforms && BIT(i))
				{
					res += map[i];
					res += "/";
				}
			}
			return res.substr(0, res.length() - 1);
		}
	}
}