#include "cl/stdafx.h"
#include "Shader.h"

#include "Context.h"

#include <d3dcompiler.h>

#include <fstream>

namespace cl {
	
	Shader::~Shader(void)
	{
		m_Desc.VS->Release();
		m_Desc.PS->Release();
	}

	void Shader::Bind(void)
	{
		Context::Instance().GetDeviceContext()->VSSetShader(m_Desc.VS, 0, 0);
		Context::Instance().GetDeviceContext()->PSSetShader(m_Desc.PS, 0, 0);
	}

	void Shader::Create(Shader* shader, const ShaderDesc& shaderDesc)
	{
		shader->m_Desc = shaderDesc;

		D3DReadFileToBlob(shaderDesc.VertexFile.c_str(), &shader->m_Desc.VSData);
		D3DReadFileToBlob(shaderDesc.PixelFile.c_str(), &shader->m_Desc.PSData);

		ASSERT(shader->m_Desc.VSData, "Couldn't read vertex shader file: \"", shader->m_Desc.VertexFile, "\"");
		ASSERT(shader->m_Desc.PSData, "Couldn't read pixel shader file: \"", shader->m_Desc.PixelFile, "\"");

		HR(Context::Instance().GetDevice()->CreateVertexShader(shader->m_Desc.VSData->GetBufferPointer(), shader->m_Desc.VSData->GetBufferSize(), NULL, &shader->m_Desc.VS));
		HR(Context::Instance().GetDevice()->CreatePixelShader(shader->m_Desc.PSData->GetBufferPointer(), shader->m_Desc.PSData->GetBufferSize(), NULL, &shader->m_Desc.PS));
	}
}