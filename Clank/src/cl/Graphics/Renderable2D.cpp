#include "cl/stdafx.h"
#include "Renderable2D.h"

#include "Renderer2D.h"

namespace cl {
	
	Renderable2D::Renderable2D(const Rectangle& rectangle, u32 color)
		: bounds(rectangle), color(color), texture(NULLPTR)
	{
		ResetUVs();
	}
	
	Renderable2D::Renderable2D(const vec2& position, const vec2& size, u32 color)
		: bounds(position, size), color(color), texture(NULLPTR)
	{
		ResetUVs();
	}
	
	Renderable2D::Renderable2D(const Rectangle& rectangle, Texture* texture, u32 color)
		: bounds(rectangle), texture(texture), color(color)
	{
		ResetUVs();
	}
	
	Renderable2D::Renderable2D(const vec2& position, const vec2& size, Texture* texture, u32 color)
		: bounds(position, size), texture(texture), color(color)
	{
		ResetUVs();
	}
	
	void Renderable2D::Submit(Renderer2D* renderer)
	{
		renderer->Submit(this);
	}
	
	void Renderable2D::ResetUVs()
	{
		uvs[0] = vec2(0.0f, 0.0f);
		uvs[1] = vec2(1.0f, 0.0f);
		uvs[2] = vec2(0.0f, 1.0f);
		uvs[3] = vec2(1.0f, 1.0f);
	}
}