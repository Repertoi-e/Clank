#pragma once

#include "cl/api.h"
#include "cl/types.h"

namespace cl {

	class API Allocator
	{
	public:
		static void* Allocate(u32 size, const char* file, u32 lin);
		static void Free(void* block);
	};
}