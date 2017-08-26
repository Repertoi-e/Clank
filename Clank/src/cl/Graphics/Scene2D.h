#pragma once

#include "cl/stdafx.h"

#include "Render/Renderer2D.h"
#include "Camera/OrthographicCamera.h"

namespace cl {

	class Scene2D
	{
	private:
		OrthographicCamera* m_Camera;
		
		Renderer2D* m_Renderer = anew Renderer2D;
		std::vector<Renderable2D*> m_Renderables;
	public:
		Scene2D(const mat4 projectionMatrix);


	};
}