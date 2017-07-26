#include "cl/stdafx.h"
#include "Renderer2D.h"

namespace cl {

	struct MatrixBufferType
	{
		mat4 Model;
		mat4 View;
		mat4 Projection;
	};

	Renderer2D::Renderer2D(void)
		: m_Context(&Context::Instance()), m_Shader(new Shader), m_VertexBuffer(new Buffer), m_Indices(0), 
		m_MatrixBuffer(new Buffer), m_IndexBuffer(new Buffer)
	{
	}

	Renderer2D::~Renderer2D(void)
	{
		delete m_Shader;
		delete m_VertexBuffer;
		delete m_IndexBuffer;
		delete m_MatrixBuffer;
	}

	void Renderer2D::Create(void)
	{
		m_Shader->Create(L"VS_R2D.cso", L"PS_R2D.cso");
		m_Shader->Bind();

		m_VertexBuffer->Create(BufferUsage::DYNAMIC, BufferBindFlag::VERTEX_BUFFER, m_Settings.BufferSize, BufferCPUA::WRITE);

		InputLayout layout;
		layout.Push<vec3>("POSITION");
		layout.Push<vec2>("TEXCOORD");
		layout.Push<u32>("ID");
		layout.Push<byte>("COLOR", 4);

		ID3D10Blob* vsData = m_Shader->GetData().vs;
		m_VertexBuffer->SetInputLayout(layout, vsData->GetBufferPointer(), vsData->GetBufferSize());

		u32* indices = new u32[m_Settings.MaxIndices];

		u32 offset = 0;
		for (u32 i = 0; i < m_Settings.MaxIndices; i += 6)
		{
			indices[i] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;

			indices[i + 3] = offset + 1;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 2;

			offset += 4;
		}

		m_IndexBuffer->Create(BufferUsage::DEFAULT, BufferBindFlag::INDEX_BUFFER, sizeof(u32) * m_Settings.MaxIndices, BufferCPUA::ZERO, indices);
		
		m_MatrixBuffer->Create(BufferUsage::DYNAMIC, BufferBindFlag::CONSTANT_BUFFER, sizeof(MatrixBufferType), BufferCPUA::WRITE);
	}

	u32 Renderer2D::HandleTexture(Texture* texture)
	{
		for (u32 i = 0; i < m_Textures.size(); i++)
		{
			if (m_Textures[i] == texture)
				return i;
		}
		if (m_Textures.size() == 16)
		{
			End();
			Present();
			Begin();
		}

		m_Textures.push_back(texture);
		return m_Textures.size() - 1;
	}

	void Renderer2D::SetCamera(Camera* camera)
	{
		m_Camera = camera;
	}

	void Renderer2D::Begin(void)
	{
		m_Map = cast(Vertex*) m_VertexBuffer->Map(BufferMapCPUA::WRITE_DISCARD);
	}

	void Renderer2D::Submit(Renderable* renderable)
	{
		Renderable2D* r = (Renderable2D*)renderable; 
		
		const Rectangle& bounds = r->bounds;
		const u32 color = r->color;
		vec2* uvs = r->uvs;

		u32 tid = 0;

		Texture* texture = r->texture;
		if (texture)
			tid = HandleTexture(texture);

		vec2 min = bounds.GetMin();
		vec2 max = bounds.GetMax();

		m_Map->position = { min.x, min.y, 0.0f };
		m_Map->uv = *uvs++;
		m_Map->tid = tid;
		m_Map->color = color;
		m_Map++;

		m_Map->position = { max.x, min.y, 0.0f };
		m_Map->uv = *uvs++;
		m_Map->tid = tid;
		m_Map->color = color;
		m_Map++;

		m_Map->position = { min.x, max.y, 0.0f };
		m_Map->uv = *uvs++;
		m_Map->tid = tid;
		m_Map->color = color;
		m_Map++;

		m_Map->position = { max.x, max.y, 0.0f };
		m_Map->uv = *uvs++;
		m_Map->tid = tid;
		m_Map->color = color;
		m_Map++;

		m_Indices += 6;
	}

	void Renderer2D::End(void)
	{
		m_VertexBuffer->Unmap();
	}

	float32 degrees = 0;
	void Renderer2D::Present(void)
	{
		degrees += 1;

		MatrixBufferType matrices;
		matrices.Model = mat4::Identity(); // mat4::Translate(vec3(0.7f, 0.3, 0)) * mat4::Rotate(degrees, vec3(0, 0, 1)); // mat4::Rotate(degrees, vec3(0, 0, 1));
		matrices.View = mat4::Identity();
		matrices.Projection = mat4::Orthographic(-400, 400, -300, 300, -1.0f, 1.0f);

		void* matData = m_MatrixBuffer->Map(BufferMapCPUA::WRITE_DISCARD);
		memcpy(matData, &matrices, sizeof(MatrixBufferType));
		m_MatrixBuffer->Unmap();

		m_MatrixBuffer->VSSet(0);

		for (u32 i = 0; i < m_Textures.size(); i++)
			m_Textures[i]->Bind(i);

		m_IndexBuffer->BindIB();
		m_VertexBuffer->BindVB(sizeof(Vertex), 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_Context->GetDeviceContext()->DrawIndexed(m_Indices, 0, 0);

		for (u32 i = 0; i < m_Textures.size(); i++)
			m_Textures[i]->Unbind(i);

		m_Indices = 0;
	}
}