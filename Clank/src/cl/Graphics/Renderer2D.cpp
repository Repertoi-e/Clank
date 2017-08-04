#include "cl/stdafx.h"
#include "Renderer2D.h"

#include "Renderable2D.h"

namespace cl {

	Renderer2D::Renderer2D(void)
		: m_Context(&Context::Instance()), m_Shader(new Shader), m_VertexBuffer(new Buffer), m_Indices(0), 
		m_MatrixBuffer(new Buffer), m_IndexBuffer(new Buffer), m_Matrices(new Matrices)
	{
	}

	Renderer2D::~Renderer2D(void)
	{
		delete m_Shader;
		delete m_VertexBuffer;
		delete m_IndexBuffer;
		delete m_MatrixBuffer;
		delete m_Matrices;
	}

	void Renderer2D::Create(void)
	{
		PushMatrix(mat4::Identity(), true);

		m_Shader->Create(L"VS_R2D.cso", L"PS_R2D.cso");
		m_Shader->Bind();

		m_VertexBuffer->Create(BufferUsage::DYNAMIC, BufferBindFlag::VERTEX_BUFFER, m_Settings.BufferSize, BufferCPUA::WRITE);

		m_Matrices->View = mat4::Identity();
		m_Matrices->Projection = mat4::Identity();

		InputLayout layout;
		layout.Push<vec4>("POSITION");
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
		
		m_MatrixBuffer->Create(BufferUsage::DYNAMIC, BufferBindFlag::CONSTANT_BUFFER, sizeof(Matrices), BufferCPUA::WRITE);
		UpdateMatrixBuffer();
	}

	u32 Renderer2D::HandleTexture(Texture* texture)
	{
		u32 result = 0;
		bool found = false;
		for (u32 i = 0; i < m_Textures.size(); i++)
		{
			if (m_Textures[i] == texture)
			{
				result = i + 1;
				found = true;
				break;
			}
		}

		if (!found)
		{
			if (m_Textures.size() >= 16)
			{
				End();
				Present();
				Begin();
			}
			m_Textures.push_back(texture);
			result = m_Textures.size();
		}
		return result;
	}

	void Renderer2D::SetCamera(Camera* camera)
	{
		m_Camera = camera;
	}

	void Renderer2D::PushMatrix(const mat4& matrix, bool override)
	{
		m_TransformationStack.push_back(override ? matrix : m_TransformationStack.back() * matrix);
		m_TransformationBack = &m_TransformationStack.back();
	}

	void Renderer2D::PopMatrix()
	{
		if (m_TransformationStack.size() <= 1)
		{	
			LOG_WARN("Trying to pop matrix but size of stack is <= 1!");
			return;
		}
		m_TransformationStack.pop_back();
		m_TransformationBack = &m_TransformationStack.back();
	}

	void Renderer2D::Begin(void)
	{
		m_Map = cast(Vertex*) m_VertexBuffer->Map(BufferMapCPUA::WRITE_DISCARD);
	}

	void Renderer2D::Submit(Renderable2D* renderable)
	{
		Renderable2D* r = (Renderable2D*)renderable; 
		
		const Rectangle& bounds = r->bounds;
		vec2 min = bounds.GetMin();
		vec2 max = bounds.GetMax();

		vec2* uvs = r->uvs;

		Texture* texture = r->texture;
		u32 tid = texture ? HandleTexture(texture) : 0;
		
		const u32 color = r->color;

		vec4 vertex = vec4(min.x, min.y, 0.0f, 1.0f);
		m_Map->position = *m_TransformationBack * vertex;
		m_Map->uv = *uvs++;
		m_Map->tid = tid;
		m_Map->color = color;
		m_Map++;

		vertex = vec4(max.x, min.y, 0.0f, 1.0f);
		m_Map->position = *m_TransformationBack * vertex;
		m_Map->uv = *uvs++;
		m_Map->tid = tid;
		m_Map->color = color;
		m_Map++;

		vertex = vec4(min.x, max.y, 0.0f, 1.0f);
		m_Map->position = *m_TransformationBack * vertex;
		m_Map->uv = *uvs++;
		m_Map->tid = tid;
		m_Map->color = color;
		m_Map++;

		vertex = vec4(max.x, max.y, 0.0f, 1.0f);
		m_Map->position = *m_TransformationBack * vertex;
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

	void Renderer2D::Present(void)
	{
		m_Matrices->Projection = m_Camera->GetProjectionMatrix();
		m_Matrices->View = m_Camera->GetViewMatrix();
		UpdateMatrixBuffer();

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

	void Renderer2D::UpdateMatrixBuffer()
	{
		void* matData = m_MatrixBuffer->Map(BufferMapCPUA::WRITE_DISCARD);
		memcpy(matData, m_Matrices, sizeof(Matrices));
		m_MatrixBuffer->Unmap();
	}
}