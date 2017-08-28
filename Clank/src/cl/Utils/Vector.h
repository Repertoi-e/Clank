#pragma once

#include "cl/stdafx.h"

namespace cl {

	class API Vector
	{
	private:
		void* m_Items;

		u32 m_Capacity = 1u;
		u32 m_Size = 0u;
		u32 m_ItemSize;
	public:
		Vector(u32 itemSize);
		~Vector(void);

		void* Get(u32 index);
		void* Front(void);
		void* Back(void);

		void Clear(void);

		void EraseRange(u32 first, u32 last);
		void Erase(u32 index);

		void Reserve(u32 size);

		void Set(u32 index, const void* item);
		void Insert(u32 index, const void* item);
		void PushBack(const void* item);

		inline u32 Size(void) const { return m_Size; }
	};
}