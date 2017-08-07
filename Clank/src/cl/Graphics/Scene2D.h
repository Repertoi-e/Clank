#pragma once

#include "cl/stdafx.h"

#include "Renderer2D.h"
#include "OrthographicCamera.h"

namespace cl {

	class Scene2D
	{
	private:
		OrthographicCamera* m_Camera;
		
		Renderer2D* m_Renderer;
		std::vector<Renderable2D*> m_Renderables;
	public:
		Scene2D(const mat4 projectionMatrix);


	};
}