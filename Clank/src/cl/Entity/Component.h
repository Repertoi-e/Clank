#pragma once

#include "cl/stdafx.h"

namespace cl {

	enum ComponentType
	{

	};

	class Component
	{
	public:
		Component() 
		{
		}

		virtual ComponentType GetType() = 0;
	};
}