#pragma once

#include "cl/api.h"
#include "cl/types.h"

namespace cl {

	class API Allocator
	{
	public:
		static void* Allocate(u32 size);
		static void* Allocate(u32 size, const char* file, u32 line);
		
		static void Free(void* block);
	};
}