#pragma once

#include "Renderer.h"

#include "cl/Graphics/Shader.h"
#include "cl/Graphics/Context.h"
#include "cl/Graphics/Buffer.h"
#include "cl/Graphics/Texture.h"
#include "cl/Graphics/Camera/Camera.h"

namespace cl {

	struct API Vertex
	{
		vec4 position;
		vec2 uv;
		u32 tid;
		u32 color;
	};

	struct API Renderer2DDesc
	{
		u32 MaxQuads;
		u32 MaxVertices;
		u32 BufferSize;
		u32 MaxIndices;
		String VertexShaderFile, PixelShaderFile;
	};

	struct Matrices
	{
		mat4 View;
		mat4 Projection;
	};

	class Renderable2D;
	
	class API Renderer2D : public Renderer
	{
	private:
		std::vector<mat4> m_TransformationStack;
		const mat4* m_TransformationBack;

		Context* m_Context = &Context::Instance();
		Shader* m_Shader = anew Shader;

		Vertex* m_Map;
		Buffer* m_VertexBuffer = anew Buffer, *m_IndexBuffer = anew Buffer, *m_MatrixBuffer = anew Buffer;
		u32 m_Indices = 0u;

		Matrices* m_Matrices = anew Matrices;

		Camera* m_Camera;

		std::vector<Texture*> m_Textures;

		Renderer2DDesc m_Desc;
	public:
		Renderer2D(void);
		~Renderer2D(void);

		u32 HandleTexture(Texture* texture);

		void SetCamera(Camera* camera);

		void PushMatrix(const mat4& matrix, bool override = false);
		void PopMatrix();

		void Begin(void) override;

		void Submit(Renderable2D* renderable);
		void DrawLine(const vec2& p1, const vec2& p2, u32 color, float32 thickness);

		void End(void) override;
		void Present(void) override;

		void UpdateMatrixBuffer();
	public:
		static void Create(Renderer2D* renderer, Renderer2DDesc& rendererDesc);
	};
}