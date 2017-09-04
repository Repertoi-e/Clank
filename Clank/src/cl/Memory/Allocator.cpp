#include "cl/stdafx.h"
#include "Allocator.h"

#include <malloc.h>

#include "cl/System/Logger.h"

namespace cl {
	
	void* Allocator::Allocate(u32 size)
	{
		ASSERT(size < 1024 * 1024 * 1024, "Allocation > 1gb");
		
		u32 actualSize = size + sizeof(u32);
		byte* result = cast(byte*) _aligned_malloc(actualSize, 16);
		memset(result, 0, actualSize);
		memcpy(result, &size, sizeof(u32));
		result += sizeof(u32);
		return result;
	}

	void* Allocator::Allocate(u32 size, const char* file, u32 line)
	{
		ASSERT(size < 1024 * 1024 * 1024, "Allocation > 1gb");

		if (size > 1024 * 1024)
		{
			DEBUG_LOG(WARN, "Allocation larger than 1 MB at ", file, " [", line, "]\n");
		}

		return Allocate(size);
	}

	void Allocator::Free(void* block)
	{
		if (!block)
			return;

		byte* memory = (cast(byte*) block) - sizeof(u32);
		u32 size = *cast(u32*) memory;
		_aligned_free(memory);
	}
}