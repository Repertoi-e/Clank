#include "cl/stdafx.h"
#include "Folder.h"

#include "Renderer2D.h"

namespace cl {

	Folder::Folder(const mat4& transformationMatrix)
		: m_TransformationMatrix(transformationMatrix)
	{
	}

	Renderable2D* Folder::Add(Renderable2D* renderable)
	{
		m_Renderables.PushBack(&renderable);
		return renderable;
	}

	void Folder::Submit(Renderer2D* renderer)
	{
		renderer->PushMatrix(m_TransformationMatrix, false);
		
		for (u32 i = 0; i < m_Renderables.Size(); i++)
			(*m_Renderables[i])->Submit(renderer);

		renderer->PopMatrix();
	}
}