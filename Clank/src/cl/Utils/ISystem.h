#pragma once

#include "cl/stdafx.h"

namespace cl {

	class API ISystem
	{
	public:
		virtual void Start() = 0;
		virtual void Shutdown() = 0;
	};
}