#include "cl/stdafx.h"
#include "Buffer.h"

#include "Context.h"

namespace cl {

	Buffer::Buffer(void)
		: m_pBuffer(NULLPTR), m_pMappedSubresource(new D3D11_MAPPED_SUBRESOURCE), m_pDesc(new D3D11_BUFFER_DESC), m_pInputLayout(NULLPTR)
	{
	}

	Buffer::~Buffer(void)
	{
		m_pBuffer->Release();
	}

	void Buffer::Create(BufferUsage usage, BufferBindFlag bindflag, u32 size, BufferCPUA access, D3D11_SUBRESOURCE_DATA* initialData)
	{
		ZeroMemory(m_pDesc, sizeof(D3D11_BUFFER_DESC));

		m_pDesc->Usage = BufferUsageToD3D(usage);
		m_pDesc->ByteWidth = size;
		m_pDesc->BindFlags = BufferBindFlagToD3D(bindflag);
		m_pDesc->CPUAccessFlags = BufferCPUAToD3D(access);

		HR(Context::Instance().GetDevice()->CreateBuffer(m_pDesc, initialData, &m_pBuffer));
	}

	void Buffer::Destroy(void)
	{
		m_pBuffer->Release();
	}

	void Buffer::SetInputLayout(InputLayout layout, void* vsbuffer, u32 vsbufferSize)
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
		HR(Context::Instance().GetDevice()->CreateInputLayout(ied, 2, vsbuffer, vsbufferSize, &m_pInputLayout));
	}

	void* Buffer::Map(BufferMapCPUA access)
	{
		HR(Context::Instance().GetDeviceContext()->Map(m_pBuffer, NULL, BufferMapCPUAToD3D(access), NULL, m_pMappedSubresource));
		
		return m_pMappedSubresource->pData;
	}

	void Buffer::Unmap(void)
	{
		Context::Instance().GetDeviceContext()->Unmap(m_pBuffer, NULL);
	}

	void Buffer::BindVB(u32 stride, u32 offset, D3D_PRIMITIVE_TOPOLOGY topology)
	{
		ID3D11DeviceContext* devcon = Context::Instance().GetDeviceContext();
		devcon->IASetVertexBuffers(0, 1, &m_pBuffer, &stride, &offset);
		devcon->IASetPrimitiveTopology(topology);
		devcon->IASetInputLayout(m_pInputLayout);
	}

	void Buffer::VSSet(u32 position, u32 buffers)
	{
		Context::Instance().GetDeviceContext()->VSSetConstantBuffers(position, buffers, &m_pBuffer);
	}

	void Buffer::BindIB(u32 offset, DXGI_FORMAT format)
	{
		Context::Instance().GetDeviceContext()->IASetIndexBuffer(m_pBuffer, format, offset);
	}

	D3D11_USAGE Buffer::BufferUsageToD3D(BufferUsage usage)
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

	u32 Buffer::BufferCPUAToD3D(BufferCPUA access)
	{
		switch (access)
		{
		case BufferCPUA::ZERO:
			return 0;
		case BufferCPUA::READ:
			return D3D11_CPU_ACCESS_READ;
		case BufferCPUA::WRITE:
			return D3D11_CPU_ACCESS_WRITE;
		}
		return 0;
	}

	D3D11_MAP Buffer::BufferMapCPUAToD3D(BufferMapCPUA access)
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

	D3D11_BIND_FLAG Buffer::BufferBindFlagToD3D(BufferBindFlag bindflag)
	{
		switch (bindflag)
		{
		case BufferBindFlag::VERTEX_BUFFER:
			return D3D11_BIND_VERTEX_BUFFER;
		case BufferBindFlag::INDEX_BUFFER:
			return D3D11_BIND_INDEX_BUFFER;
		case BufferBindFlag::CONSTANT_BUFFER:
			return D3D11_BIND_CONSTANT_BUFFER;
		case BufferBindFlag::SHADER_RESOURCE:
			return D3D11_BIND_SHADER_RESOURCE;
		case BufferBindFlag::RENDER_TARGET:
			return D3D11_BIND_RENDER_TARGET;
		case BufferBindFlag::DEPTH_STENCIL:
			return D3D11_BIND_DEPTH_STENCIL;
		}
		return D3D11_BIND_VERTEX_BUFFER;
	}
}