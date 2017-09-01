#pragma once

#include "Allocator.h"

#define anew    new
#define del		delete

#pragma warning(disable : 4595)

inline void* operator new(u32 size)
{
	return cl::Allocator::Allocate(size, "", 0);
}

inline void* operator new[](u32 size)
{
	return cl::Allocator::Allocate(size, "", 0);
}

inline void* operator new(u32 size, const char* file, u32 line)
{
	return cl::Allocator::Allocate(size, file, line);
}

inline void* operator new[](u32 size, const char* file, u32 line)
{
	return cl::Allocator::Allocate(size, file, line);
}

inline void operator delete(void* block)
{
	cl::Allocator::Free(block);
}

inline void operator delete[](void* block)
{
	cl::Allocator::Free(block);
}

inline void operator delete(void* block, const char* file, u32 line)
{
	cl::Allocator::Free(block);
}

inline void operator delete[](void* block, const char* file, u32 line)
{
	cl::Allocator::Free(block);
}
