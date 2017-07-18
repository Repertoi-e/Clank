#include "cl/stdafx.h"
#include "Renderer2D.h"

namespace cl {

	Renderer2D::Renderer2D(void)
		: m_pContext(&Context::Instance()), m_pShader(new Shader()), m_pVertexBuffer(new VertexBuffer()), m_Indices(0)
	{
	}

	Renderer2D::~Renderer2D(void)
	{
		delete m_pShader;
	}

	void Renderer2D::Create(void)
	{
		m_pShader->Create(L"res/Vertex.hlsl", L"res/Pixel.hlsl");
		m_pShader->Bind();

		m_pVertexBuffer->Create(BufferUsage::DYNAMIC, m_Settings.BufferSize, BufferCPUA::WRITE);

		InputLayout layout;
		layout.Push<vec3>("POSITION");
		layout.Push<byte>("COLOR", 4);

		ID3D10Blob* vsData = m_pShader->GetData().vs;
		m_pVertexBuffer->SetInputLayout(layout, vsData->GetBufferPointer(), vsData->GetBufferSize());
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

		m_pMap->position = { max.x, min.y, 0.0f };
		m_pMap->color = color;
		m_pMap++;

		m_pMap->position = { max.x, max.y, 0.0f };
		m_pMap->color = color;
		m_pMap++;

		m_pMap->position = { min.x, max.y, 0.0f };
		m_pMap->color = color;
		m_pMap++;

		m_Indices += 6;
	}

	void Renderer2D::End(void)
	{
		m_pVertexBuffer->Unmap();
	}

	void Renderer2D::Present(void)
	{
		m_pVertexBuffer->Bind(sizeof(Vertex), 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pContext->GetDeviceContext()->Draw(m_Indices, 0);

		m_Indices = 0;
	}
}