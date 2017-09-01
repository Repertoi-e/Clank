#pragma once

#include "cl/stdafx.h"

#include <d3d11.h>

namespace cl {

	struct API ShaderDesc
	{
		String VertexFile;
		String PixelFile;
		ID3D11VertexShader* VS = NULLPTR;
		ID3D11PixelShader* PS = NULLPTR;
		ID3D10Blob* VSData = NULLPTR;
		ID3D10Blob* PSData = NULLPTR;
	};

	class API Shader
	{
	private:
		ShaderDesc m_Desc;
	public:
		~Shader(void);

		void Bind(void);

		inline ShaderDesc& GetDesc() { return m_Desc; }
		inline const ShaderDesc& GetDesc() const { return m_Desc; }
	public:
		static void Create(Shader* shader, const ShaderDesc& shaderDesc);
	};
}