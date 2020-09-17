#pragma once

// contrary to asserts, traps are enabled in release builds, even for tools and resource compiler
#define ENABLE_TRAPS	!__FINAL

#if ENABLE_TRAPS

#define TRAP_ONLY(...)  __VA_ARGS__

#define TrapEQ(x,y)		(Likely((x)!=(y)) || (__debugbreak(),0))
#define TrapNE(x,y)		(Likely((x)==(y)) || (__debugbreak(),0))
#define TrapGE(x,y)		(Likely((x)<(y))  || (__debugbreak(),0))
#define TrapGT(x,y)		(Likely((x)<=(y)) || (__debugbreak(),0))
#define TrapLE(x,y)		(Likely((x)>(y))  || (__debugbreak(),0))
#define TrapLT(x,y)		(Likely((x)>=(y)) || (__debugbreak(),0))
#define TrapZ(x)		(Likely((x)!=0)   || (__debugbreak(),0))
#define TrapNZ(x)		(Likely((x)==0)   || (__debugbreak(),0))

#else // ENABLE_TRAPS

#define TRAP_ONLY(...)

#define TrapEQ(x,y)
#define TrapNE(x,y)
#define TrapGE(x,y)
#define TrapGT(x,y)
#define TrapLE(x,y)
#define TrapLT(x,y)
#define TrapZ(x)
#define TrapNZ(x)

#endif // ENABLE_TRAPS