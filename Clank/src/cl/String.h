#pragma once

#include "cl/api.h"

namespace cl {

	struct StringBuffer
	{
		u32 Size = 1024;
		u32 Occupied = 0;
		wchar* Data = anew wchar[Size];

		~StringBuffer(void);

		// Converts chars to wide chars 
		void Append(const char* str);
		void Append(const wchar* str);

		void AppendChar(wchar ch);
	private:
		void HandleMemory(u32 size);
	};
}