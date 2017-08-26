#pragma once

#include "cl/stdafx.h"

#include "Camera.h"

namespace cl {

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(const mat4& orthographicMatrix);
	};
}