#pragma once

#include "cl/stdafx.h"

#include "cl/Maths/maths.h"

namespace cl {

	class API Camera
	{
	protected:
		mat4 m_ProjectionMatrix;
		mat4 m_ViewMatrix;
	public:
		Camera(const mat4& projectionMatrix, const mat4& viewMatrix);

		inline const mat4& GetProjectionMatrix() { return m_ProjectionMatrix; }
		inline const mat4& GetViewMatrix() { return m_ViewMatrix; }
	};
}