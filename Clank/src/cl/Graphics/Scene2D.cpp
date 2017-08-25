#include "cl/stdafx.h"
#include "Scene2D.h"

namespace cl {

	Scene2D::Scene2D(const mat4 projectionMatrix)
		: m_Camera(anew OrthographicCamera(projectionMatrix))
	{
	}
}