#pragma once

#include "cl/types.h"

namespace cl {

	struct SmartString
	{
		wchar* String;
		bool Owned;

		SmartString(const wchar* data, bool owned);

		void TryFree(void);
	};

	struct StringBuffer
	{
		u32 Size = 1024;
		u32 Occupied = 0;
		wchar* Data;

		StringBuffer(void);
		~StringBuffer(void);

		// Converts chars to wide chars 
		void AppendString(const char* str, u32 len = -1);
		void AppendString(const wchar* str, u32 len = -1);

		void AppendChar(wchar ch);
	private:
		void HandleMemory(u32 size);
	};
}