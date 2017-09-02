#include "cl/stdafx.h"
#include "Buffer.h"

#include "Context.h"

namespace cl {

	void InputLayout::Push(const char* name, Format type, u32 count)
	{
		u32 size = FormatToSize(type);
		
		InputElement element = { name, type, size, count, m_Size };
		m_Elements.PushBack(&element);
		m_Size += size * count;
	}

	u32 InputLayout::FormatToSize(Format format)
	{
		switch (format)
		{
		case Format::R32_FLOAT:
			return sizeof(float32);
		case Format::RG32_FLOAT:
			return sizeof(float32) * 2;
		case Format::RGB32_FLOAT:
			return sizeof(float32) * 3;
		case Format::RGBA32_FLOAT:
			return sizeof(float32) * 4;
		case Format::R32_UINT:
			return sizeof(u32);
		case Format::RGBA8_UNORM:
			return sizeof(byte);
		}

		ASSERT(false, "Unsupported format.");
		
		return 0;
	}

	Buffer::Buffer(void)
	{
	}

	Buffer::~Buffer(void)
	{
		m_Buffer->Release();
	}

	Buffer* Buffer::Create(Buffer* buffer, BufferDesc& bufferDesc)
	{
		buffer->m_Desc = bufferDesc;

		D3D11_BUFFER_DESC desc;
		{
			ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		
			desc.Usage = BufferUsageToD3D(buffer->m_Desc.Usage);
			desc.ByteWidth = buffer->m_Desc.Size;
			desc.BindFlags = BufferBindFlagToD3D(buffer->m_Desc.BindFlag);
			desc.CPUAccessFlags = BufferAccessToD3D(buffer->m_Desc.Access);
		}
		
		D3D11_SUBRESOURCE_DATA* indataptr = NULLPTR;
		if (buffer->m_Desc.InitialData)
		{
			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = buffer->m_Desc.InitialData;
			initData.SysMemPitch = 0;
			initData.SysMemSlicePitch = 0;
			indataptr = &initData;
		}

		HR(Context::Instance().GetDevice()->CreateBuffer(&desc, indataptr, &buffer->m_Buffer));

		return buffer;
	}

	void Buffer::Destroy(void)
	{
		m_Buffer->Release();
	}

	void Buffer::SetInputLayout(const InputLayout& layout, void* vsbuffer, u32 vsbufferSize)
	{
		m_Layout = layout;

		if (m_Layout.m_InputLayout)
			m_Layout.m_InputLayout->Release();

		Vector<InputElement>& elements = m_Layout.GetElements();

		D3D11_INPUT_ELEMENT_DESC* ied = anew D3D11_INPUT_ELEMENT_DESC[elements.Size()];
		for (u32 i = 0; i < elements.Size(); i++)
		{
			InputElement* element = cast(InputElement*) elements.Get(i);
			ied[i] = { element->name, 0, FormatToD3D(element->type), 0, element->offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		}
		HR(Context::Instance().GetDevice()->CreateInputLayout(ied, elements.Size(), vsbuffer, vsbufferSize, &m_Layout.m_InputLayout));
	}

	void* Buffer::Map(BufferMapAccess access)
	{
		HR(Context::Instance().GetDeviceContext()->Map(m_Buffer, NULL, BufferMapAccessToD3D(access), NULL, m_MappedResource));
		
		return m_MappedResource->pData;
	}

	void Buffer::Unmap(void)
	{
		Context::Instance().GetDeviceContext()->Unmap(m_Buffer, NULL);
	}

	void Buffer::BindVB(u32 stride, u32 offset, D3D_PRIMITIVE_TOPOLOGY topology)
	{
		ID3D11DeviceContext* devcon = Context::Instance().GetDeviceContext();
		devcon->IASetVertexBuffers(0, 1, &m_Buffer, &stride, &offset);
		devcon->IASetPrimitiveTopology(topology);
		devcon->IASetInputLayout(m_Layout.m_InputLayout);
	}

	void Buffer::VSSet(u32 position, u32 buffers)
	{
		Context::Instance().GetDeviceContext()->VSSetConstantBuffers(position, buffers, &m_Buffer);
	}

	void Buffer::BindIB(u32 offset, DXGI_FORMAT format)
	{
		Context::Instance().GetDeviceContext()->IASetIndexBuffer(m_Buffer, format, offset);
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

	u32 Buffer::BufferAccessToD3D(BufferAccess access)
	{
		switch (access)
		{
		case BufferAccess::ZERO:
			return 0;
		case BufferAccess::READ:
			return D3D11_CPU_ACCESS_READ;
		case BufferAccess::WRITE:
			return D3D11_CPU_ACCESS_WRITE;
		}
		return 0;
	}

	D3D11_MAP Buffer::BufferMapAccessToD3D(BufferMapAccess access)
	{
		switch (access)
		{
		case BufferMapAccess::READ:
			return D3D11_MAP_READ;
		case BufferMapAccess::WRITE:
			return D3D11_MAP_WRITE;
		case BufferMapAccess::READ_WRITE:
			return D3D11_MAP_READ_WRITE;
		case BufferMapAccess::WRITE_DISCARD:
			return D3D11_MAP_WRITE_DISCARD;
		case BufferMapAccess::WRITE_NO_OVERWRITE:
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