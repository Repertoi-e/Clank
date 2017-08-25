#pragma once

#include "cl/stdafx.h"

#include <d3d11.h>

namespace cl {

	struct API ShaderDesc
	{
		String VertexFile;
		String PixelFile;
	};

	class API Shader
	{
	public:
		struct Data
		{
			ID3D10Blob* vs;
			ID3D10Blob* ps;
		};
	private:
		Data m_Data;
		ID3D11VertexShader* m_VS = NULLPTR;
		ID3D11PixelShader* m_PS = NULLPTR;
	public:
		Shader(void);
		~Shader(void);

		inline Data& GetData(void) { return m_Data; }

		inline ID3D11VertexShader* GetVS(void) { return m_VS; }
		inline ID3D11PixelShader* GetPS(void) { return m_PS; }

		void Bind(void);
	public:
		static void Create(Shader* shader, ShaderDesc& shaderDesc);
	};
}