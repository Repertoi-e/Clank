#pragma once

#include "cl/stdafx.h"

#include "Component.h"

namespace cl {

	class Entity
	{
	private:
		std::unordered_map<ComponentType, Component*> m_Components;
	public:
		Entity(void);

		void Add(Component* component);
		
		template<typename T>
		Component* Get(void)
		{
			ComponentType type = T::GetType();
			auto result = m_Components.find(type);
			if (result != m_Components.end())
				return cast(T*) result;
			
			return nullptr;
		}
	};
}