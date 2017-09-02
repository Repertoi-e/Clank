#pragma once

#include "cl/stdafx.h"

#include "cl/Memory/Vector.h"

#include "cl/Graphics/Texture.h"
#include "cl/Graphics/Text/Font.h"

#include "cl/Utils/Singleton.h"

namespace cl {

	template <typename T>
	class ResourceManager : public Singleton<ResourceManager<T>>
	{
	private:
		Vector<T> m_Resources;
	public:
		ResourceManager()
		{
		}

		~ResourceManager()
		{
		}

		void Add(T* t)
		{
			ASSERT(t, "Resource is NULLPTR");
		
			m_Resources.PushBack(t);
		}

		void Remove(T* t)
		{
			ASSERT(t, "Resource is NULLPTR");

			for (u32 i = 0; i < m_Resources.Size(); i++)
			{
				if (m_Resources.Get(i) == t)
					m_Resources.Erase(i);
			}
		}

		T* Get(String name)
		{
			for (u32 i = 0; i < m_Resources.Size(); i++)
			{
				T* resource = m_Resources.Get(i);
				if (resource->GetDesc().Name == name)
					return resource;
			}

			return NULLPTR;
		}
	};

	extern API ResourceManager<Font>& g_Fonts;
	extern API ResourceManager<Texture>& g_Textures;
}