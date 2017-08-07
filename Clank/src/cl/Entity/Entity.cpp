#include "cl/stdafx.h"
#include "Entity.h"

namespace cl {

	Entity::Entity()
	{
	}

	void Entity::Add(Component* component)
	{
		m_Components[component->GetType()] = component;
	}
}