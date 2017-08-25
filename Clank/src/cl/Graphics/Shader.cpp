#include "cl/stdafx.h"
#include "Shader.h"

#include "Context.h"

#include <d3dcompiler.h>

#include <fstream>

namespace cl {
	
	Shader::Shader(void)
	{
		m_Data.vs = NULLPTR;
		m_Data.ps = NULLPTR;
	}
	
	Shader::~Shader(void)
	{
		m_VS->Release();
		m_PS->Release();
	}

	void Shader::Create(Shader* shader, ShaderDesc& shaderDesc)
	{
		D3DReadFileToBlob(shaderDesc.VertexFile.c_str(), &shader->m_Data.vs);
		D3DReadFileToBlob(shaderDesc.PixelFile.c_str(), &shader->m_Data.ps);

		ASSERT(shader->m_Data.vs, "Couldn't read vertex shader file: \"", shaderDesc.VertexFile, "\"");
		ASSERT(shader->m_Data.ps, "Couldn't read pixel shader file: \"", shaderDesc.PixelFile, "\"");

		HR(Context::Instance().GetDevice()->CreateVertexShader(shader->m_Data.vs->GetBufferPointer(), shader->m_Data.vs->GetBufferSize(), NULL, &shader->m_VS));
		HR(Context::Instance().GetDevice()->CreatePixelShader(shader->m_Data.ps->GetBufferPointer(), shader->m_Data.ps->GetBufferSize(), NULL, &shader->m_PS));
	}

	void Shader::Bind(void)
	{
		Context::Instance().GetDeviceContext()->VSSetShader(m_VS, 0, 0);
		Context::Instance().GetDeviceContext()->PSSetShader(m_PS, 0, 0);
	}
}