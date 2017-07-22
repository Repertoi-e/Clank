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
		ZERO,
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

	enum class BufferBindFlag
	{
		VERTEX_BUFFER,
		INDEX_BUFFER,
		CONSTANT_BUFFER,
		SHADER_RESOURCE,
		RENDER_TARGET,
		DEPTH_STENCIL
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

	class API Buffer
	{
	private:
		ID3D11Buffer* m_pBuffer;
		D3D11_MAPPED_SUBRESOURCE* m_pMappedSubresource;
		D3D11_BUFFER_DESC* m_pDesc;
		ID3D11InputLayout* m_pInputLayout;

		InputLayout m_Layout;
	public:
		Buffer();
		~Buffer();

		void Create(BufferUsage usage, BufferBindFlag bindflag, u32 size, BufferCPUA access, D3D11_SUBRESOURCE_DATA* initialData = NULLPTR);
		void Destroy();

		void SetInputLayout(InputLayout layout, void* vsbuffer, u32 vsbufferSize);

		void* Map(BufferMapCPUA access);
		void Unmap();

		// Bind the buffer as a vertex buffer
		void BindVB(u32 stride, u32 offset, D3D_PRIMITIVE_TOPOLOGY topology);

		// Bind the buffer as a constant vertex shader buffer
		void VSSet(u32 position, u32 buffers = 1);

		// Bind the buffer as an index buffer
		void BindIB(u32 offset = 0, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);

		static D3D11_USAGE BufferUsageToD3D(BufferUsage usage);
		static u32 BufferCPUAToD3D(BufferCPUA access);
		static D3D11_MAP BufferMapCPUAToD3D(BufferMapCPUA access);
		static D3D11_BIND_FLAG BufferBindFlagToD3D(BufferBindFlag bindflag);
	};
}