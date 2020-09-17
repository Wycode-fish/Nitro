#pragma once

#include <string>
#include <memory>

#define UASSERT(x)	((x) || (__debugbreak(), 0))

// @ the reason why this macro token paste needs to be in this complicated way is:
// @	to evaluate the actual value of input arg 'x' & 'y' first before pasting.
#define MACRO_DOJOIN2(x, y) x##y
#define MACRO_DOJOIN(x, y) MACRO_DOJOIN2(x, y)
#define MACRO_JOIN(x, y) MACRO_DOJOIN(x, y)
// @ a trick to do static assert, the compiler will report negative subscript(-1) for failure of assertion
#define COMPILETIME_ASSERT(Cond) \
typedef char MACRO_JOIN( MACRO_JOIN( __NT_CompileTime_Assert, __LINE__ ), __)[(Cond)?1:-1]

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

	typedef u64 hashkey_t;
}