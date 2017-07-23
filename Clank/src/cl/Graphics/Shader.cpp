#include "cl/stdafx.h"
#include "Shader.h"

#include "Context.h"

#include <d3dcompiler.h>

#include <fstream>

namespace cl {
	
	Shader::Shader(void)
		: m_pVS(NULLPTR), m_pPS(NULLPTR)
	{
		m_Data.vs = NULLPTR;
		m_Data.ps = NULLPTR;
	}
	
	Shader::~Shader(void)
	{
		m_pVS->Release();
		m_pPS->Release();
	}

	void Shader::Create(LPCWSTR vertSrc, LPCWSTR pixelSrc)
	{
		D3DReadFileToBlob(vertSrc, &m_Data.vs);
		D3DReadFileToBlob(pixelSrc, &m_Data.ps);

		ASSERT(m_Data.vs, "Couldn't read vertex shader file: \"", vertSrc,"\"");
		ASSERT(m_Data.ps, "Couldn't read pixel shader file: \"", pixelSrc, "\"");

		HR(Context::Instance().GetDevice()->CreateVertexShader(m_Data.vs->GetBufferPointer(), m_Data.vs->GetBufferSize(), NULL, &m_pVS));
		HR(Context::Instance().GetDevice()->CreatePixelShader(m_Data.ps->GetBufferPointer(), m_Data.ps->GetBufferSize(), NULL, &m_pPS));
	}

	void Shader::Bind(void)
	{
		Context::Instance().GetDeviceContext()->VSSetShader(m_pVS, 0, 0);
		Context::Instance().GetDeviceContext()->PSSetShader(m_pPS, 0, 0);
	}
}