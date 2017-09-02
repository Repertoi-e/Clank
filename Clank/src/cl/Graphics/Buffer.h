#pragma once

#include "cl/stdafx.h"

#include "cl/Maths/maths.h"
#include "cl/Memory/Vector.h"

#include "D3DTypes.h"

#include <d3d11.h>

namespace cl {

	enum class BufferUsage : u8
	{				/// CPU:           GPU:
		DEFAULT,	//  None		   Read / Write
		IMMUTABLE,	//  None		   Read Only
		DYNAMIC,	//  Write Only	   Read Only
		STAGING		//  Read / Write   Read / Write
	};

	enum class BufferAccess : u8
	{
		ZERO,
		READ,
		WRITE
	};

	enum class BufferMapAccess : u8
	{
		READ,				// Buffer can only be read by the CPU.
		WRITE,				// Buffer can only be written to by the CPU.
		READ_WRITE,			// Buffer can be read and written to by the CPU.
		WRITE_DISCARD,		// Previous contents of buffer are erased, and new buffer is opened for writing.
		WRITE_NO_OVERWRITE	// An advanced flag that allows you to add more data to the buffer even while the GPU is using parts. However, you must not work with the parts the GPU is using.
	};

	enum class BufferBindFlag : u8
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
		Format type;
		u32 size;
		u32 count;
		u32 offset;
	};

	class API InputLayout
	{
	public:
		friend class Buffer;
	private:
		u32 m_Size = 0;
		ID3D11InputLayout* m_InputLayout = NULLPTR;

		Vector<InputElement> m_Elements;
	public:
		void Push(const char* name, Format type, u32 count = 1);

		inline u32 GetSize(void) const { return m_Size; }
		inline Vector<InputElement>& GetElements(void) { return m_Elements; }		
	public:
		static u32 FormatToSize(Format format);
	};

	struct API BufferDesc
	{
		BufferUsage Usage;
		BufferBindFlag BindFlag;
		u32 Size;
		BufferAccess Access;
		void* InitialData = NULLPTR;
	};

	class API Buffer
	{
	private:
		ID3D11Buffer* m_Buffer = NULLPTR;
		D3D11_MAPPED_SUBRESOURCE* m_MappedResource = anew D3D11_MAPPED_SUBRESOURCE;

		BufferDesc m_Desc;
		InputLayout m_Layout;
	public:
		Buffer(void);
		~Buffer(void);

		void Destroy(void);

		void SetInputLayout(const InputLayout& layout, void* vsbuffer, u32 vsbufferSize);

		void* Map(BufferMapAccess access);
		void Unmap(void);

		// Bind the buffer as a vertex buffer
		void BindVB(u32 stride, u32 offset, D3D_PRIMITIVE_TOPOLOGY topology);

		// Bind the buffer as a constant vertex shader buffer
		void VSSet(u32 position, u32 buffers = 1);

		// Bind the buffer as an index buffer
		void BindIB(u32 offset = 0, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);

		inline BufferDesc& GetDesc() { return m_Desc; }
		inline const BufferDesc& GetDesc() const { return m_Desc; }
	public:
		static D3D11_USAGE BufferUsageToD3D(BufferUsage usage);
		static u32 BufferAccessToD3D(BufferAccess access);
		static D3D11_MAP BufferMapAccessToD3D(BufferMapAccess access);
		static D3D11_BIND_FLAG BufferBindFlagToD3D(BufferBindFlag bindflag);
	public:
		static Buffer* Create(Buffer* buffer, BufferDesc& desc);
	};
}