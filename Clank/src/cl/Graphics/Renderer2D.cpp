#include "cl/stdafx.h"
#include "Renderer2D.h"

#include "Renderable2D.h"

namespace cl {

	Renderer2D::Renderer2D(void)
		: m_Context(&Context::Instance()), m_Shader(cl_new Shader), m_VertexBuffer(cl_new Buffer), m_Indices(0),
		m_MatrixBuffer(cl_new Buffer), m_IndexBuffer(cl_new Buffer), m_Matrices(cl_new Matrices)
	{
	}

	Renderer2D::~Renderer2D(void)
	{
		cl_delete m_Shader;
		cl_delete m_VertexBuffer;
		cl_delete m_IndexBuffer;
		cl_delete m_MatrixBuffer;
		cl_delete m_Matrices;
	}

	void Renderer2D::Create(Renderer2D* renderer, Renderer2DDesc& rendererDesc)
	{
		renderer->m_Desc = rendererDesc;
		
		renderer->PushMatrix(mat4::Identity(), true);

		ShaderDesc shaderDesc;
		{
			ZeroMemory(&shaderDesc, sizeof(ShaderDesc));

			shaderDesc.VertexFile = L"VS_R2D.cso";
			shaderDesc.PixelFile = L"PS_R2D.cso";
		}
		Shader::Create(renderer->m_Shader, shaderDesc);
		
		renderer->m_Shader->Bind();

		BufferDesc vertDesc;
		{
			ZeroMemory(&vertDesc, sizeof(BufferDesc));

			vertDesc.Usage = BufferUsage::DYNAMIC;
			vertDesc.BindFlag = BufferBindFlag::VERTEX_BUFFER;
			vertDesc.Size = rendererDesc.BufferSize;
			vertDesc.Access = BufferAccess::WRITE;
		}
		Buffer::Create(renderer->m_VertexBuffer, vertDesc);

		renderer->m_Matrices->View = mat4::Identity();
		renderer->m_Matrices->Projection = mat4::Identity();

		InputLayout layout;
		layout.Push<vec4>("POSITION");
		layout.Push<vec2>("TEXCOORD");
		layout.Push<u32>("ID");
		layout.Push<byte>("COLOR", 4);

		ID3D10Blob* vsData = renderer->m_Shader->GetData().vs;
		renderer->m_VertexBuffer->SetInputLayout(layout, vsData->GetBufferPointer(), vsData->GetBufferSize());

		u32* indices = cl_new u32[rendererDesc.MaxIndices];

		u32 offset = 0;
		for (u32 i = 0; i < rendererDesc.MaxIndices; i += 6)
		{
			indices[i] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;

			indices[i + 3] = offset + 1;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 2;

			offset += 4;
		}

		BufferDesc indexDesc;
		{
			ZeroMemory(&indexDesc, sizeof(BufferDesc));

			indexDesc.Usage = BufferUsage::DEFAULT;
			indexDesc.BindFlag = BufferBindFlag::INDEX_BUFFER;
			indexDesc.Size = sizeof(u32) * rendererDesc.MaxIndices;
			indexDesc.Access = BufferAccess::ZERO;
			indexDesc.InitialData = indices;
		}
		Buffer::Create(renderer->m_IndexBuffer, indexDesc);

		BufferDesc matrixDesc;
		{
			ZeroMemory(&matrixDesc, sizeof(BufferDesc));

			matrixDesc.Usage = BufferUsage::DYNAMIC;
			matrixDesc.BindFlag = BufferBindFlag::CONSTANT_BUFFER;
			matrixDesc.Size = sizeof(Matrices);
			matrixDesc.Access = BufferAccess::WRITE;
		}
		Buffer::Create(renderer->m_MatrixBuffer, matrixDesc);
		
		renderer->UpdateMatrixBuffer();
	}

	u32 Renderer2D::HandleTexture(Texture* texture)
	{
		for (u32 i = 0; i < m_Textures.size(); i++)
			if (m_Textures[i] == texture)
				return i + 1;

		if (m_Textures.size() >= 16)
		{
			End();
			Present();
			Begin();
		}
		m_Textures.push_back(texture);
		return m_Textures.size();
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
		m_Map = cast(Vertex*) m_VertexBuffer->Map(BufferMapAccess::WRITE_DISCARD);
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
		memcpy(m_MatrixBuffer->Map(BufferMapAccess::WRITE_DISCARD), m_Matrices, sizeof(Matrices));
		m_MatrixBuffer->Unmap();
	}
}