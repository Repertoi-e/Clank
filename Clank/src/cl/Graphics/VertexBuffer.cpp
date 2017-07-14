#include "cl/stdafx.h"
#include "VertexBuffer.h"

#include "Context.h"

namespace cl {

	VertexBuffer::VertexBuffer()
		: m_pVBuffer(NULLPTR), m_pMappedSubresource(new D3D11_MAPPED_SUBRESOURCE()), m_pDesc(new D3D11_BUFFER_DESC), m_pInputLayout(NULLPTR)
	{
	}

	VertexBuffer::~VertexBuffer()
	{
		Destroy();
	}

	void VertexBuffer::Create(BufferUsage usage, u32 size, BufferCPUA access)
	{
		ZeroMemory(m_pDesc, sizeof(D3D11_BUFFER_DESC));

		m_pDesc->Usage = BufferUsageToD3D(usage);
		m_pDesc->ByteWidth = size;
		m_pDesc->BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_pDesc->CPUAccessFlags = BufferCPUAToD3D(access);

		Context::Instance().GetDevice()->CreateBuffer(m_pDesc, NULL, &m_pVBuffer);
	}

	void VertexBuffer::Destroy()
	{
		m_pVBuffer->Release();
	}

	void VertexBuffer::SetInputLayout(InputLayout layout, void* vsbuffer, u32 vsbufferSize)
	{
		m_Layout = std::move(layout);

		if (m_pInputLayout)
			m_pInputLayout->Release();

		const std::vector<InputElement>& elements = m_Layout.GetElements();

		D3D11_INPUT_ELEMENT_DESC* ied = new D3D11_INPUT_ELEMENT_DESC[elements.size()];
		for (u32 i = 0; i < elements.size(); i++)
		{
			const InputElement& element = elements[i];
			ied[i] = { element.name, 0, element.type, 0, element.offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		}
		Context::Instance().GetDevice()->CreateInputLayout(ied, 2, vsbuffer, vsbufferSize, &m_pInputLayout);
	}

	void* VertexBuffer::Map(BufferMapCPUA access)
	{
		Context::Instance().GetDeviceContext()->Map(m_pVBuffer, NULL, BufferMapCPUAToD3D(access), NULL, m_pMappedSubresource);
		
		return m_pMappedSubresource->pData;
	}

	void VertexBuffer::Unmap()
	{
		Context::Instance().GetDeviceContext()->Unmap(m_pVBuffer, NULL);
	}

	void VertexBuffer::Bind(u32 stride, u32 offset, D3D_PRIMITIVE_TOPOLOGY topology)
	{
		Context::Instance().GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pVBuffer, &stride, &offset);
		Context::Instance().GetDeviceContext()->IASetPrimitiveTopology(topology);
		Context::Instance().GetDeviceContext()->IASetInputLayout(m_pInputLayout);
	}

	D3D11_USAGE VertexBuffer::BufferUsageToD3D(BufferUsage usage)
	{
		switch (usage)
		{
		case BufferUsage::DEFAULT:
			return D3D11_USAGE_DEFAULT;
		case BufferUsage::DYNAMIC:
			return D3D11_USAGE_DYNAMIC;
		case BufferUsage::IMMUTABLE:
			return D3D11_USAGE_IMMUTABLE;
		case BufferUsage::STAGING:
			return D3D11_USAGE_STAGING;
		}
		return D3D11_USAGE_DEFAULT;
	}

	D3D11_CPU_ACCESS_FLAG VertexBuffer::BufferCPUAToD3D(BufferCPUA access)
	{
		switch (access)
		{
		case BufferCPUA::READ:
			return D3D11_CPU_ACCESS_READ;
		case BufferCPUA::WRITE:
			return D3D11_CPU_ACCESS_WRITE;
		}
		return D3D11_CPU_ACCESS_READ;
	}

	D3D11_MAP VertexBuffer::BufferMapCPUAToD3D(BufferMapCPUA access)
	{
		switch (access)
		{
		case BufferMapCPUA::READ:
			return D3D11_MAP_READ;
		case BufferMapCPUA::WRITE:
			return D3D11_MAP_WRITE;
		case BufferMapCPUA::READ_WRITE:
			return D3D11_MAP_READ_WRITE;
		case BufferMapCPUA::WRITE_DISCARD:
			return D3D11_MAP_WRITE_DISCARD;
		case BufferMapCPUA::WRITE_NO_OVERWRITE:
			return D3D11_MAP_WRITE_NO_OVERWRITE;
		}
		return D3D11_MAP_READ;
	}
}