#pragma once

// @ ------------------------------------------
// @			System-Setting Macros
// @ ------------------------------------------
#define NT_USE_VENDOR_MATH

#ifdef NT_PLATFORM_WINDOWS
	#ifdef NT_LNKOPT_DYNAMIC
		#ifdef NT_BUILD_DLL
			#define NITRO_API __declspec(dllexport)
		#else 
			#define NITRO_API __declspec(dllimport)
		#endif
	#else
		#define NITRO_API
	#endif	// Windows Link Options
#else
	#error Nitro is only for windows
#endif

#if NT_DEBUG
	#define NT_DEBUG_ASSERTS_ENABLED
	#define NT_DEV_INFO_ENABLED
#endif

// @ ------------------------------------------
// @			RSG Macros
// @ ------------------------------------------
#define NITRO_ENABLE_NITRO_NEW 0

#define NON_COPYABLE(Class) \
Class(const Class&) = delete;\
const Class& operator=(const Class&) = delete


// @ ------------------------------------------
// @			Global Typedef
// @ ------------------------------------------
namespace Nitro
{
	typedef unsigned char u8;

	typedef unsigned short u16;
	typedef short s16;

	typedef unsigned int u32;
	typedef int i32;

	typedef unsigned long long u64;
	typedef long long i64;

	typedef float f32;
	typedef double f64;
}