#pragma once

#include "UtilConfig.h"

#define BIT(x)				(1 << x)
#define NT_EVENT_BIND(x)	std::bind(x, this, std::placeholders::_1)

#define NT_DO_TOKEN_PASTE(a, b)		a#b
#define NT_TOKEN_PASTE(a, b)		NT_DO_TOKEN_PASTE(a, b)

#define NT_HEAP_RELEASE(x) { if (x!=nullptr) { delete x; } }

// @ --------- Windows Specific Utilities ---------
#ifdef NT_PLATFORM_WINDOWS
	#define NT_DX_RELEASE(x) { if (x) { x->Release(); (x) = 0; }}
#endif

namespace Nitro
{
	// @ ------------------------------------------
	// @			Utility Ref Ops Templates
	// @ ------------------------------------------
	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T, typename ... TArgs>
	constexpr Scope<T> CreateScope(TArgs&& ... args)
	{
		return std::make_unique<T>(std::forward<TArgs>(args)...);
	}

	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename ... TArgs>
	constexpr Ref<T> CreateRef(TArgs&& ... args)
	{
		return std::make_shared<T>(std::forward<TArgs>(args)...);
	}

	namespace Util
	{
		// @ ------------------------------------------------------------
		// @					Variable Conversion
		// @ ------------------------------------------------------------
		template <typename T>
		std::string integral_to_hexstr(T w, size_t hex_len = sizeof(T) << 1) {
			static const char* digits = "0123456789ABCDEF";
			std::string rc(hex_len, '0');
			for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
				rc[i] = digits[(w >> j) & 0x0f];
			return rc;
		}

#pragma region RSG
		template <typename L, typename R>
		const L& Assign(L& l, const R r)
		{
			l = static_cast<L>(r);
			return l;
		}

#pragma endregion
	}

}