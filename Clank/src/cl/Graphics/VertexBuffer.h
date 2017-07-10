#pragma once

#include "cl/stdafx.h"

#include <d3d11.h>

namespace cl {

	enum class BufferUsage
	{				/// CPU:           GPU:
		DEFAULT,	//  None		   Read / Write
		IMMUTABLE,	//  None		   Read Only
		DYNAMIC,	//  Write Only	   Read Only
		STAGING	//  Read / Write   Read / Write
	};

	enum class BufferCPUAccess
	{
		READ,
		WRITE
	};

	class API VertexBuffer
	{
	private:
		ID3D11Buffer* m_pVBuffer;
	public:
		VertexBuffer();
		~VertexBuffer();

		void Create(BufferUsage usage, u32 size, BufferCPUAccess access, ID3D11Device* device);
		void Destroy();

		static D3D11_USAGE BufferUsageToD3D(BufferUsage usage);
		static u32 BufferCPUAccessToD3D(BufferCPUAccess access);
	};
}