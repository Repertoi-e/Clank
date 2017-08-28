#include "cl/stdafx.h"
#include "Vector.h"

#include "Log.h"

namespace cl {

	Vector::Vector(u32 itemSize)
		: m_ItemSize(itemSize)
	{
		m_Items = malloc(m_ItemSize * m_Capacity);
	}

	Vector::~Vector()
	{
		free(m_Items);
	}

	void* Vector::Get(u32 index)
	{
		ASSERT(index < m_Size, "Index out of bounds");

		return (s8*)(m_Items) + index * m_ItemSize;
	}

	void* Vector::Front(void)
	{
		return Get(0);
	}
	
	void* Vector::Back(void)
	{
		return Get(m_Size - 1);
	}

	void Vector::Clear(void)
	{
		m_Size = 0;
	}

	void Vector::EraseRange(u32 first, u32 last)
	{
		ASSERT(first < m_Size, "Index out of bounds");
		ASSERT(last  < m_Size + 1, "Index out of bounds");
		ASSERT(first < last, "Invalid first/last in Vector::EraseRange");

		memmove((s8*)(m_Items) + first * m_ItemSize, (s8*)(m_Items) + last * m_ItemSize, (m_Size - last) * m_ItemSize);
		m_Size -= (last - first);
	}
	
	void Vector::Erase(u32 index)
	{
		EraseRange(index, index + 1);
	}

	void Vector::Reserve(u32 size)
	{
		if (m_Capacity < size)
		{
			m_Items = realloc(m_Items, size * m_ItemSize);
			m_Capacity = size;
		}
	}

	void Vector::Set(u32 index, const void* item)
	{
		ASSERT(index  < m_Size, "Index out of bounds");

		memcpy((s8*)(m_Items) + index * m_ItemSize, item, m_ItemSize);
	}

	void Vector::Insert(u32 index, const void* item)
	{
		ASSERT(index <= m_Size, "Index out of bounds");

		if (m_Capacity <= m_Size)
			Reserve(2 * m_Capacity);
		if (index < m_Size)
			memmove((s8*)(m_Items) + (index + 1) * m_ItemSize, (s8*)(m_Items) + (index + 0) * m_ItemSize, (m_Size - index)  * m_ItemSize);
		
		m_Size++;
		Set(index, item);
	}

	void Vector::PushBack(const void* item)
	{
		Insert(m_Size, item);
	}
}