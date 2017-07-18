#pragma once

#include "cl/stdafx.h"

namespace cl {

	class API ISystem
	{
	public:
		virtual void Start(void) = 0;
		virtual void Shutdown(void) = 0;
	};
}