#pragma once
#include <new>

// Alignment type to avoid conflict with other new operators
enum class align_t : size_t {};

void* operator new(size_t size, align_t align);
void* operator new[](size_t size, align_t align);

#if NITRO_ENABLE_NITRO_NEW
#include "Nitro/Framework/rsg/TLS.h"

void* operator new(size_t size, const char* file, int line, int bucket);
void* operator new[](size_t size, const char* file, int line, int bucket);

void* operator new(size_t size, align_t align, const char* file, int line, int bucket);
void* operator new[](size_t size, align_t align, const char* file, int line, int bucket);

#define nitro_new new(__FILE__, __LINE__, __BUCKET__)
#define nitro_aligned_new(a) new(align_t(a))
#define nitro_alignedof_new(a) new(align_t(__alignof(a)))
#else
#define nitro_new new
#define nitro_aligned_new(a) new(align_t(a))
#define nitro_alignedof_new(a) new(align_t(__alignof(a)))
#endif