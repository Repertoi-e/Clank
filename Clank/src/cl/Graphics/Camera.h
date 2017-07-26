#pragma once

#include "cl/stdafx.h"

#include "cl/Maths/maths.h"

namespace cl {

	class API Camera
	{
	public:
		virtual mat4 GetProjectionMatrix() = 0;
	};
}