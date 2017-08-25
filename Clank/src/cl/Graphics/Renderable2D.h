#pragma once

#include "cl/stdafx.h"

#include "Texture.h"

#include "cl/Maths/maths.h"
#include "cl/Maths2/Rectangle.h"

namespace cl {

	class Renderer2D;

	class API Renderable2D
	{
	public:
		Rectangle bounds;
		u32 color = 0xffffffff;
		Texture* texture = NULLPTR;
		vec2 uvs[4];
	protected:
		Renderable2D() {}
	public:
		Renderable2D(const Rectangle& rectangle, u32 color = 0xffffffff);
		Renderable2D(const vec2& position, const vec2& size, u32 color = 0xffffffff);
		Renderable2D(const Rectangle& rectangle, Texture* texture);
		Renderable2D(const vec2& position, const vec2& size, Texture* texture);

		virtual void Submit(Renderer2D* renderer);

		void ResetUVs();
	};
}