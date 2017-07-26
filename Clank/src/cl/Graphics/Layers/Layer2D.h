#pragma once

#include "Layer.h"

#include "cl/Graphics/Renderer2D.h"

namespace cl {

	class Layer2D : public Layer
	{
	private:
		mat4 m_ProjectionMatrix;
		Renderer2D* m_Renderer;
	public:
		Layer2D(mat4 projectionMatrix)
			: m_ProjectionMatrix(projectionMatrix), m_Renderer(new Renderer2D)
		{
		}

		void OnInit(void) override;
		virtual void OnEvent(Event& event) override = 0;
		void OnRender(void) override;
		virtual void OnUpdate(const DeltaTime& dt) override = 0;
		virtual void OnTick(void) override = 0;

		virtual void Init(Context* context, Renderer2D* renderer) = 0;
		virtual void OnRender(Context* context, Renderer2D* renderer) = 0;
	};
}