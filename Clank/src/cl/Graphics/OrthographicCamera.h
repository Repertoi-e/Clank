#pragma once

#include "cl/stdafx.h"

#include "Camera.h"

namespace cl {

	class OrthographicCamera : public Camera
	{
	private:
		mat4 m_OrthographicMatrix;
	public:
		OrthographicCamera(mat4 orthographicMatrix)
			: m_OrthographicMatrix(orthographicMatrix)
		{
		}

		inline mat4 GetProjectionMatrix() override { return m_OrthographicMatrix; }
	};
}