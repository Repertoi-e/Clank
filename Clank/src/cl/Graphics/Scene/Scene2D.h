#pragma once

#include "cl/stdafx.h"

#include "Scene.h"
#include "cl/Graphics/Render/Renderer2D.h"
#include "cl/Graphics/Render/Renderable2D.h"
#include "cl/Graphics/Camera/OrthographicCamera.h"

namespace cl {

	class Scene2D : public Scene
	{
	protected:
		OrthographicCamera* m_Camera;
		
		Renderer2D* m_Renderer = anew Renderer2D;
		Vector<Renderable2D*> m_Renderables;
	protected:
		Scene2D(const mat4 projectionMatrix);
		~Scene2D(void);
	private:
		virtual void OnRender(void) override;
	protected:
		virtual void OnPush(void) override {}
		virtual void OnPop(void) override {}
		
		virtual void OnEvent(Event& event) override {}
		
		virtual void PreRender(Renderer2D* renderer, Context* context) {}
		virtual void EarlyRender(Renderer2D* renderer, Context* context) {}
		virtual void LateRender(Renderer2D* renderer, Context* context) {}
		virtual void PostRender(Renderer2D* renderer, Context* context) {}

		virtual void OnUpdate(const DeltaTime& dt) override {}
		virtual void OnTick(void) override {}
	public:
		Renderable2D* Add(Renderable2D* renderable);
		void Remove(Renderable2D* renderable);
	};
}