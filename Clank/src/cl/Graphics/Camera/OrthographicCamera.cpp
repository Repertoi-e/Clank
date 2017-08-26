#include "cl/stdafx.h"
#include "OrthographicCamera.h"

namespace cl {

	OrthographicCamera::OrthographicCamera(const mat4& orthographicMatrix)
		: Camera(orthographicMatrix, mat4::Identity())
	{
	}
}