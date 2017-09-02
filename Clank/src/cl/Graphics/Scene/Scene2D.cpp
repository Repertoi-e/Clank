#include "cl/stdafx.h"
#include "Scene2D.h"

namespace cl {

	Scene2D::Scene2D(const mat4 projectionMatrix)
		: m_Camera(anew OrthographicCamera(projectionMatrix))
	{
	}

	Scene2D::~Scene2D(void)
	{
		del m_Renderer;
		del m_Camera;
	}

	void Scene2D::OnRender(void)
	{
		PreRender(m_Renderer, &Context::Instance());

		m_Renderer->Begin();

		EarlyRender(m_Renderer, &Context::Instance());

		for (u32 i = 0; i < m_Renderables.Size(); i++)
			(*m_Renderables[i])->Submit(m_Renderer);

		LateRender(m_Renderer, &Context::Instance());

		m_Renderer->End();
		m_Renderer->Present();

		PostRender(m_Renderer, &Context::Instance());
	}

	Renderable2D* Scene2D::Add(Renderable2D* renderable)
	{
		m_Renderables.PushBack(&renderable);
		
		return renderable;
	}

	void Scene2D::Remove(Renderable2D* renderable)
	{
		for (u32 i = 0; i < m_Renderables.Size(); i++)
			if (*m_Renderables[i] == renderable)
				m_Renderables.Erase(i);
	}
}