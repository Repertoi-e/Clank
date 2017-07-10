#include "cl/stdafx.h"
#include "VertexBuffer.h"

namespace cl {

	VertexBuffer::VertexBuffer()
		: m_pVBuffer(NULLPTR)
	{
	}

	VertexBuffer::~VertexBuffer()
	{
		Destroy();
	}

	void VertexBuffer::Create(BufferUsage usage, u32 size, BufferCPUAccess access, ID3D11Device* device)
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = BufferUsageToD3D(usage);
		bd.ByteWidth = size;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = BufferCPUAccessToD3D(access);

		device->CreateBuffer(&bd, NULL, &m_pVBuffer);
	}

	void VertexBuffer::Destroy()
	{
		m_pVBuffer->Release();
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

	u32 VertexBuffer::BufferCPUAccessToD3D(BufferCPUAccess access)
	{
		switch (access)
		{
		case BufferCPUAccess::READ:
			return D3D11_CPU_ACCESS_READ;
		case BufferCPUAccess::WRITE:
			return D3D11_CPU_ACCESS_WRITE;
		}
		return 0;
	}
}