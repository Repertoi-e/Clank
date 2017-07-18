#pragma once

#include "cl/stdafx.h"

#include "cl/Maths/maths.h"

#include <d3d11.h>

namespace cl {

	enum class BufferUsage
	{				/// CPU:           GPU:
		DEFAULT,	//  None		   Read / Write
		IMMUTABLE,	//  None		   Read Only
		DYNAMIC,	//  Write Only	   Read Only
		STAGING		//  Read / Write   Read / Write
	};

	enum class BufferCPUA
	{
		READ,
		WRITE
	};

	enum class BufferMapCPUA
	{
		READ,				// Buffer can only be read by the CPU.
		WRITE,				// Buffer can only be written to by the CPU.
		READ_WRITE,			// Buffer can be read and written to by the CPU.
		WRITE_DISCARD,		// Previous contents of buffer are erased, and new buffer is opened for writing.
		WRITE_NO_OVERWRITE	// An advanced flag that allows you to add more data to the buffer even while the GPU is using parts. However, you must not work with the parts the GPU is using.
	};

	struct API InputElement
	{
		const char* name;
		DXGI_FORMAT type;
		u32 size;
		u32 count;
		u32 offset;
	};

	class API InputLayout
	{
	private:
		u32 m_Size;
		std::vector<InputElement> m_Layout;
	public:
		InputLayout(void)
			: m_Size(0)
		{
		}

		template<typename T>
		void Push(const char* name, u32 count = 1)
		{
		}

		template<>
		void Push<float>(const char* name, u32 count)
		{
			Push(name, DXGI_FORMAT_R32_FLOAT, sizeof(float), count);
		}

		template<>
		void Push<u32>(const char* name, u32 count)
		{
			Push(name, DXGI_FORMAT_R32_UINT, sizeof(u32), count);
		}

		template<>
		void Push<byte>(const char* name, u32 count)
		{
			Push(name, DXGI_FORMAT_R8G8B8A8_UNORM, sizeof(byte), count);
		}

		template<>
		void Push<vec2>(const char* name, u32 count)
		{
			Push(name, DXGI_FORMAT_R32G32_FLOAT, sizeof(vec2), count);
		}

		template<>
		void Push<vec3>(const char* name, u32 count)
		{
			Push(name, DXGI_FORMAT_R32G32B32_FLOAT, sizeof(vec3), count);
		}

		template<>
		void Push<vec4>(const char* name, u32 count)
		{
			Push(name, DXGI_FORMAT_R32G32B32A32_FLOAT, sizeof(vec4), count);
		}

		inline const std::vector<InputElement>& GetElements(void) const { return m_Layout; }
		inline u32 GetSize(void) const { return m_Size; }
	private:
		void Push(const char* name, DXGI_FORMAT type, u32 size, u32 count)
		{
			m_Layout.push_back({ name, type, size, count, m_Size });
			m_Size += size * count;
		}
	};

	class API VertexBuffer
	{
	private:
		ID3D11Buffer* m_pVBuffer;
		D3D11_MAPPED_SUBRESOURCE* m_pMappedSubresource;
		D3D11_BUFFER_DESC* m_pDesc;
		ID3D11InputLayout* m_pInputLayout;

		InputLayout m_Layout;
	public:
		VertexBuffer();
		~VertexBuffer();

		void Create(BufferUsage usage, u32 size, BufferCPUA access);
		void Destroy();
		
		void SetInputLayout(InputLayout layout, void* vsbuffer, u32 vsbufferSize);

		void* Map(BufferMapCPUA access);
		void Unmap();

		void Bind(u32 stride, u32 offset, D3D_PRIMITIVE_TOPOLOGY topology);

		static D3D11_USAGE BufferUsageToD3D(BufferUsage usage);
		static D3D11_CPU_ACCESS_FLAG BufferCPUAToD3D(BufferCPUA access);
		static D3D11_MAP BufferMapCPUAToD3D(BufferMapCPUA access);
	};
}