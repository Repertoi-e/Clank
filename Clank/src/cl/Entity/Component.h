#pragma once

#include "cl/stdafx.h"

namespace cl {

	enum ComponentType
	{

	};

	class Component
	{
	public:
		Component(void)
		{
		}

		virtual ComponentType GetType(void) = 0;
	};
}