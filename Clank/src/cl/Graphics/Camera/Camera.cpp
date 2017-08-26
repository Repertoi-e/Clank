#include "cl/stdafx.h"
#include "Camera.h"

namespace cl {

	Camera::Camera(const mat4& projectionMatrix, const mat4& viewMatrix)
		: m_ProjectionMatrix(projectionMatrix), m_ViewMatrix(viewMatrix)
	{
	}
}