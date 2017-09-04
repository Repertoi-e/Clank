#pragma once

#include "cl/stdafx.h"

#include "cl/System/Logger.h"

namespace cl {

	template <typename T>
	class API Vector
	{
	private:
		void* m_Items;

		u32 m_Capacity = 1u;
		u32 m_Size = 0u;
		u32 m_ItemSize;
	public:
		Vector(void);
		~Vector(void);

		T* Get(u32 index);
		T* Front(void);
		T* Back(void);

		void Clear(void);
		void EraseRange(u32 first, u32 last);
		void Erase(u32 index);

		void Reserve(u32 size);
		void Set(u32 index, const T* item);

		void Insert(u32 index, const T* item);
		void PushBack(const T* item);

		inline u32 Size(void) const { return m_Size; }

		T* operator[](u32 index)
		{
			return Get(index);
		}
	};
	
	template<typename T>
	inline Vector<T>::Vector(void)
		: m_ItemSize(sizeof(T))
	{
		m_Items = malloc(m_ItemSize * m_Capacity);
	}
	
	template<typename T>
	inline Vector<T>::~Vector(void)
	{
		free(m_Items);
	}
	
	template<typename T>
	inline T* Vector<T>::Get(u32 index)
	{
		ASSERT(index < m_Size, "Index out of bounds");

		return cast(T*) (cast(s8*) (m_Items) + index * m_ItemSize);
	}
	
	template<typename T>
	inline T* Vector<T>::Front(void)
	{
		return Get(0);
	}
	
	template<typename T>
	inline T* Vector<T>::Back(void)
	{
		return Get(m_Size - 1);
	}
	
	template<typename T>
	inline void Vector<T>::Clear(void)
	{
		m_Size = 0;
	}
	
	template<typename T>
	inline void Vector<T>::EraseRange(u32 first, u32 last)
	{
		ASSERT(first < m_Size, "Index out of bounds");
		ASSERT(last  < m_Size + 1, "Index out of bounds");
		ASSERT(first < last, "Invalid first/last in Vector::EraseRange");

		memmove(cast(s8*) (m_Items) + first * m_ItemSize, cast(s8*) (m_Items) + last * m_ItemSize, (m_Size - last) * m_ItemSize);
		m_Size -= (last - first);
	}
	
	template<typename T>
	inline void Vector<T>::Erase(u32 index)
	{
		EraseRange(index, index + 1);
	}
	
	template<typename T>
	inline void Vector<T>::Reserve(u32 size)
	{
		if (m_Capacity < size)
		{
			m_Items = realloc(m_Items, size * m_ItemSize);
			m_Capacity = size;
		}
	}
	
	template<typename T>
	inline void Vector<T>::Set(u32 index, const T* item)
	{
		ASSERT(index  < m_Size, "Index out of bounds");

		memcpy(cast(s8*) (m_Items) + index * m_ItemSize, item, m_ItemSize);
	}
	
	template<typename T>
	inline void Vector<T>::Insert(u32 index, const T* item)
	{
		ASSERT(index <= m_Size, "Index out of bounds");

		if (m_Capacity <= m_Size)
			Reserve(2 * m_Capacity);
		if (index < m_Size)
			memmove(cast(s8*) (m_Items) + (index + 1) * m_ItemSize, cast(s8*) (m_Items) + (index + 0) * m_ItemSize, (m_Size - index)  * m_ItemSize);

		m_Size++;
		Set(index, item);
	}
	
	template<typename T>
	inline void Vector<T>::PushBack(const T* item)
	{
		Insert(m_Size, item);
	}
}