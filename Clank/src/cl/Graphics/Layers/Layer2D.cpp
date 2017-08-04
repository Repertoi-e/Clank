#include "cl/stdafx.h"
#include "Layer2D.h"

#include "cl/Graphics/Renderable2D.h"

namespace cl {

	Renderable2D* Layer2D::Add(Renderable2D* renderable)
	{
		m_Renderables.push_back(renderable);
		return renderable;
	}

	void Layer2D::OnInit(void)
	{
		Init(&Context::Instance(), m_Renderer);
	}

	void Layer2D::OnRender(void)
	{
		PreRender(&Context::Instance(), m_Renderer);

		m_Renderer->Begin();

		EarlyRender(&Context::Instance(), m_Renderer);

		for (auto r : m_Renderables)
			r->Submit(m_Renderer);

		OnRender(&Context::Instance(), m_Renderer);

		m_Renderer->End();
		m_Renderer->Present();

		PostRender(&Context::Instance(), m_Renderer);
	}
}