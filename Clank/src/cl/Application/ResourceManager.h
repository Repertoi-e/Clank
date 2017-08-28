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
		Vector* m_Resources;
	public:
		ResourceManager()
		{
			m_Resources = anew Vector(sizeof(T));
		}

		~ResourceManager()
		{
			del m_Resources;
		}

		void Add(T* t)
		{
			m_Resources->PushBack(t);
		}

		void Remove(T* t)
		{
			for (u32 i = 0; i < m_Resources->Size(); i++)
			{
				if (m_Resources->Get(i) == t)
					m_Resources->Erase(i);
			}
		}

		T* Get(String name)
		{
			for (u32 i = 0; i < m_Resources->Size(); i++)
			{
				T* resource = m_Resources->Get(i);
				if (resource->GetName() == name)
					return resource;
			}
		}
	};

	extern API ResourceManager<Font>& g_Fonts;
	extern API ResourceManager<Texture>& g_Textures;
}