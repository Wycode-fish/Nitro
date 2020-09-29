#pragma once

#define NT_USE_VENDOR_MATH 1

#define INLINE					__forceinline
#define DX_ALIGNMENT_DEFAULT	16
#define NT_ALIGN				__declspec(align(DX_ALIGNMENT_DEFAULT))

enum EZeroTag { kZero, kOrigin };
enum EIdentityTag { kOne, kIdentity };
enum EXUnitVector { kXUnitVector };
enum EYUnitVector { kYUnitVector };
enum EZUnitVector { kZUnitVector };
enum EWUnitVector { kWUnitVector };


typedef unsigned char u8;

typedef unsigned short u16;
typedef short s16;

typedef unsigned int u32;
typedef int i32;

typedef unsigned long long u64;
typedef long long i64;

typedef float f32;
typedef double f64;

// @ NOT READY TO SUPPORT MULTIPLE MATH LIBS HERE, 
// @ DIRECTLY USE GLM FOR NOW
#ifdef NT_USE_VENDOR_MATH
	#ifndef NT_WINDOWED_APP
		#include "glm/glm.hpp"
		#define M128	__m128	
		#define FVEC3	glm::vec3
		#define FVEC4	glm::vec4
		#define FMAT4	glm::mat4
	#else
		#include <DirectXMath.h>
		using namespace DirectX;
		#define M128	XMVECTOR
		#define FVEC3	XMFLOAT3
		#define FVEC4	XMFLOAT4
		#define FMAT4	XMMATRIX
	#endif
#endif