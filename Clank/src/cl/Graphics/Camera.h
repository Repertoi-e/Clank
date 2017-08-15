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

		inline void SetProjectionMatrix(const mat4& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }
		inline void SetViewMatrix(const mat4& viewMatrix) { m_ViewMatrix = viewMatrix; }

		inline const mat4& GetProjectionMatrix(void) { return m_ProjectionMatrix; }
		inline const mat4& GetViewMatrix(void) { return m_ViewMatrix; }
	};
}