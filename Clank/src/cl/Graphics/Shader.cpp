#include "cl/stdafx.h"
#include "Shader.h"

#include "Context.h"

#include <d3dcompiler.h>

#include <fstream>

namespace cl {
	
	Shader::Shader()
		: m_pVS(NULLPTR), m_pPS(NULLPTR)
	{
		m_Data.vs = NULLPTR;
		m_Data.ps = NULLPTR;
	}
	
	Shader::~Shader()
	{
		m_pVS->Release();
		m_pPS->Release();
	}
	
	void Shader::Create(LPCWSTR vertSrc, LPCWSTR fragSrc)
	{
		ID3DBlob* errorBlob;

		D3DCompileFromFile(vertSrc, 0, 0, "main", "vs_4_0", D3DCOMPILE_DEBUG, 0, &m_Data.vs, &errorBlob);
		if (errorBlob)
		{
			if (errorBlob->GetBufferSize())
				std::cout << "Shader Compile Errors" << std::endl << (const char*)errorBlob->GetBufferPointer() << std::endl;
			errorBlob->Release();
		}

		D3DCompileFromFile(fragSrc, 0, 0, "main", "ps_4_0", D3DCOMPILE_DEBUG, 0, &m_Data.ps, &errorBlob);
		if (errorBlob)
		{
			if (errorBlob->GetBufferSize())
				std::cout << "Shader Compile Errors" << std::endl << (const char*)errorBlob->GetBufferPointer() << std::endl;
			errorBlob->Release();
		}

		Context::Instance().GetDevice()->CreateVertexShader(m_Data.vs->GetBufferPointer(), m_Data.vs->GetBufferSize(), NULL, &m_pVS);
		Context::Instance().GetDevice()->CreatePixelShader(m_Data.ps->GetBufferPointer(), m_Data.ps->GetBufferSize(), NULL, &m_pPS);
	}

	void Shader::Bind()
	{
		Context::Instance().GetDeviceContext()->VSSetShader(m_pVS, 0, 0);
		Context::Instance().GetDeviceContext()->PSSetShader(m_pPS, 0, 0);
	}
}