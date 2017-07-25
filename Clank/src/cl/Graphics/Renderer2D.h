#pragma once

#include "Renderer.h"
#include "Shader.h"
#include "Context.h"
#include "Buffer.h"
#include "Texture.h"

#include "cl/Maths/maths.h"
#include "cl/Maths2/Rectangle.h"

namespace cl {

	struct API Vertex
	{
		vec3 position;
		vec2 uv;
		u32 tid;
		u32 color;
	};

	class API Renderable2D : public Renderable
	{
	public:
		Rectangle bounds;
		u32 color;
		Texture* texture;
	public:
		Renderable2D(const Rectangle& rectangle, u32 color = 0xffffffff)
			: bounds(rectangle), color(color), texture(NULLPTR)
		{
		}

		Renderable2D(const vec2& position, const vec2& size, u32 color = 0xffffffff)
			: bounds(position, size), color(color), texture(NULLPTR)
		{
		}

		Renderable2D(const Rectangle& rectangle, Texture* texture, u32 color = 0xffffffff)
			: bounds(rectangle), texture(texture), color(color)
		{
		}

		Renderable2D(const vec2& position, const vec2& size, Texture* texture, u32 color = 0xffffffff)
			: bounds(position, size), texture(texture), color(color)
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
		Context* m_Context;
		Shader* m_Shader;

		Vertex* m_Map;
		Buffer* m_VertexBuffer, *m_IndexBuffer, *m_MatrixBuffer;
		u32 m_Indices;

		std::vector<Texture*> m_Textures;

		Renderer2DSettings m_Settings;
	public:
		Renderer2D(void);
		~Renderer2D(void);

		void Create(void);

		u32 HandleTexture(Texture* texture);

		void Begin(void) override;
		void Submit(Renderable* renderable) override;
		void End(void) override;
		void Present(void) override;

		inline void SetSettings(Renderer2DSettings settings) { m_Settings = std::move(settings); }
	};
}