#include "cl/stdafx.h"
#include "Folder.h"

#include "cl/Graphics/Render/Renderer2D.h"

namespace cl {

	Folder::Folder(const mat4& transformationMatrix)
		: m_TransformationMatrix(transformationMatrix)
	{
	}

	Renderable2D* Folder::Add(Renderable2D* renderable)
	{
		m_Renderables.push_back(renderable);
		return renderable;
	}

	void Folder::Submit(Renderer2D* renderer)
	{
		renderer->PushMatrix(m_TransformationMatrix, false);
		
		for (auto r : m_Renderables)
			r->Submit(renderer);

		renderer->PopMatrix();
	}
}