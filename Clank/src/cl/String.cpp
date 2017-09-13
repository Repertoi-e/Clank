#include "cl/stdafx.h"

namespace cl {

	SmartString::SmartString(const wchar* data, bool owned)
		: Owned(owned)
	{
		if (Owned)
		{
			String = anew wchar[wcslen(data)];
			wcscpy(String, data);
		}
		else
			String = cast(wchar*) data;
	}

	void SmartString::TryFree(void)
	{
		if (Owned)
			del String;
	}

	StringBuffer::StringBuffer(void)
		: Data(anew wchar[Size])
	{
	}
	
	StringBuffer::~StringBuffer(void)
	{
		del Data;
	}
	
	void StringBuffer::AppendString(const char* str, u32 len)
	{
		wchar buffer[1024 * 10] = { 0 };
		mbstowcs(buffer, str, strlen(str) + 1);
		AppendString(buffer, len);
	}
	
	void StringBuffer::AppendString(const wchar* str, u32 len)
	{
		s32 size = len == cast(u32) -1 ? wcslen(str) : len;
		
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