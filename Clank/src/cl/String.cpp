#include "cl/stdafx.h"
#include "String.h"

namespace cl {

	StringBuffer::~StringBuffer(void)
	{
		del Data;
	}
	
	void StringBuffer::Append(const char* str)
	{
		wchar buffer[1024 * 10] = { 0 };
		mbstowcs(buffer, str, strlen(str) + 1);
		Append(buffer);
	}
	
	void StringBuffer::Append(const wchar* str)
	{
		s32 size = wcslen(str);
		
		HandleMemory(size);

		wmemcpy(&Data[Occupied], str, size);
		Occupied += size;
	}

	void StringBuffer::AppendChar(wchar ch)
	{
		HandleMemory(1);

		Data[Occupied++] = ch;
	}

	void StringBuffer::HandleMemory(u32 size)
	{
		if (Occupied + size > Size)
		{
			Size *= 2;
			del Data;
			Data = anew wchar[Size];
		}
	}
}