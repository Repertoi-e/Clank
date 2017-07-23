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
		: m_pContext(&Context::Instance()), m_pShader(new Shader), m_pVertexBuffer(new Buffer), m_Indices(0), 
		m_pMatrixBuffer(new Buffer), m_pIndexBuffer(new Buffer)
	{
	}

	Renderer2D::~Renderer2D(void)
	{
		delete m_pShader;
	}

	void Renderer2D::Create(void)
	{
		m_pShader->Create(L"data/VS_R2D.cso", L"data/PS_R2D.cso");
		m_pShader->Bind();

		m_pVertexBuffer->Create(BufferUsage::DYNAMIC, BufferBindFlag::VERTEX_BUFFER, m_Settings.BufferSize, BufferCPUA::WRITE);

		InputLayout layout;
		layout.Push<vec3>("POSITION");
		layout.Push<byte>("COLOR", 4);

		ID3D10Blob* vsData = m_pShader->GetData().vs;
		m_pVertexBuffer->SetInputLayout(layout, vsData->GetBufferPointer(), vsData->GetBufferSize());

		u32 indices[] = 
		{
			0, 1, 2,
			1, 3, 2
		};

		D3D11_SUBRESOURCE_DATA* indexData = new D3D11_SUBRESOURCE_DATA;
		indexData->pSysMem = indices;
		indexData->SysMemPitch = 0;
		indexData->SysMemSlicePitch = 0;

		m_pIndexBuffer->Create(BufferUsage::DEFAULT, BufferBindFlag::INDEX_BUFFER, sizeof(indices), BufferCPUA::ZERO, indexData);
		
		m_pMatrixBuffer->Create(BufferUsage::DYNAMIC, BufferBindFlag::CONSTANT_BUFFER, sizeof(MatrixBufferType), BufferCPUA::WRITE);
	}

	void Renderer2D::Begin(void)
	{
		m_pMap = cast(Vertex*) m_pVertexBuffer->Map(BufferMapCPUA::WRITE_DISCARD);
	}

	void Renderer2D::Submit(Renderable* renderable)
	{
		Renderable2D* r = (Renderable2D*)renderable; 
		
		const Rectangle& bounds = r->bounds;
		const u32 color = r->color;

		vec2 min = bounds.GetMin();
		vec2 max = bounds.GetMax();

		m_pMap->position = { min.x, min.y, 0.0f };
		m_pMap->color = color;
		m_pMap++;

		m_pMap->position = { max.x, min.y, 0.0f };
		m_pMap->color = color;
		m_pMap++;

		m_pMap->position = { min.x, max.y, 0.0f };
		m_pMap->color = color;
		m_pMap++;

		m_pMap->position = { max.x, max.y, 0.0f };
		m_pMap->color = color;
		m_pMap++;

		m_Indices += 6;
	}

	void Renderer2D::End(void)
	{
		m_pVertexBuffer->Unmap();
	}

	float32 degrees = 0;
	void Renderer2D::Present(void)
	{
		degrees += 1;

		MatrixBufferType matrices;
		matrices.Model = mat4::Translate(vec3(0.7f, 0.3, 0)) * mat4::Rotate(degrees, vec3(0, 0, 1)); // mat4::Rotate(degrees, vec3(0, 0, 1));
		matrices.View = mat4::Identity();
		matrices.Projection = mat4::Orthographic(-400, 400, -300, 300, -1.0f, 1.0f);

		void* matData = m_pMatrixBuffer->Map(BufferMapCPUA::WRITE_DISCARD);
		memcpy(matData, &matrices, sizeof(MatrixBufferType));
		m_pMatrixBuffer->Unmap();

		m_pMatrixBuffer->VSSet(0);

		m_pIndexBuffer->BindIB();
		m_pVertexBuffer->BindVB(sizeof(Vertex), 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pContext->GetDeviceContext()->DrawIndexed(m_Indices, 0, 0);

		m_Indices = 0;
	}
}