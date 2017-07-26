#include "cl/stdafx.h"
#include "Layer2D.h"

namespace cl {

	void Layer2D::OnInit(void)
	{
		Init(&Context::Instance(), m_Renderer);
	}

	void Layer2D::OnRender(void)
	{
		OnRender(&Context::Instance(), m_Renderer);
	}
}