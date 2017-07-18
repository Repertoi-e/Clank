#pragma once

#include "Renderer.h"
#include "Shader.h"
#include "Context.h"
#include "VertexBuffer.h"

#include "cl/Maths/maths.h"
#include "cl/Maths2/Rectangle.h"

namespace cl {

	struct API Vertex
	{
		vec3 position;
		u32 color;
	};

	class API Renderable2D : public Renderable
	{
	public:
		Rectangle bounds;
		u32 color;
	public:
		Renderable2D(const Rectangle& rectangle, u32 color = 0xff00ffff)
			: bounds(rectangle), color(color)
		{
		}

		Renderable2D(const vec2& position, const vec2& size, u32 color = 0xff00ffff)
			: bounds(position, size), color(color)
		{
		}
	};

	struct API Renderer2DSettings
	{
		u32 MaxQuads;
		u32 MaxVertices;
		u32 BufferSize;
		u32 MaxIndices;
	};
	
	class API Renderer2D : public Renderer
	{
	private:
		Context* m_pContext;
		Shader* m_pShader;

		Vertex* m_pMap;
		VertexBuffer* m_pVertexBuffer;
		u32 m_Indices;

		Renderer2DSettings m_Settings;
	public:
		Renderer2D(void);
		~Renderer2D(void);

		void Create(void);

		void Begin(void) override;
		void Submit(Renderable* renderable) override;
		void End(void) override;
		void Present(void) override;

		inline void SetSettings(Renderer2DSettings settings) { m_Settings = std::move(settings); }
	};
}