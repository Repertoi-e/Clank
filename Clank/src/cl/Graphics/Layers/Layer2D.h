#pragma once

#include "Layer.h"

#include "cl/Graphics/Renderer2D.h"

namespace cl {

	class Layer2D : public Layer
	{
	protected:
		mat4 m_ProjectionMatrix;
		Renderer2D* m_Renderer;

		std::vector<Renderable2D*> m_Renderables;
	public:
		Layer2D(mat4 projectionMatrix)
			: m_ProjectionMatrix(projectionMatrix), m_Renderer(cl_new Renderer2D)
		{
		}

		Renderable2D* Add(Renderable2D* renderable);

		void OnInit(void) override;
		virtual void OnEvent(Event& event) override {}
		void OnRender(void) override;
		virtual void OnUpdate(const DeltaTime& dt) override {}
		virtual void OnTick(void) override {}

		virtual void Init(Context* context, Renderer2D* renderer) = 0;
		virtual void PreRender(Context* context, Renderer2D* renderer) {}
		virtual void OnRender(Context* context, Renderer2D* renderer) {}
		virtual void PostRender(Context* context, Renderer2D* renderer) {}
		virtual void EarlyRender(Context* context, Renderer2D* renderer) {}
	};
}