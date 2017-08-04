#pragma once

#include "cl/stdafx.h"

#include "Renderable2D.h"

namespace cl {

	class API Folder : public Renderable2D
	{
	private:
		mat4 m_TransformationMatrix;

		std::vector<Renderable2D*> m_Renderables;
	public:
		Folder(const mat4& transformationMatrix);

		Renderable2D* Add(Renderable2D* renderable);

		void Submit(Renderer2D* renderer) override;

		inline const mat4& GetTransformationMatrix() { return m_TransformationMatrix; }
		inline void SetTransformationMatrix(const mat4& transformationMatrix) { m_TransformationMatrix = transformationMatrix; }
	};
}