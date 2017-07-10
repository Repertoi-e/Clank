#include "cl/stdafx.h"
#include "Shader.h"

#include <d3dcompiler.h>

namespace cl {
	
	Shader::Shader()
	{
	}
	
	Shader::~Shader()
	{
		m_pVS->Release();
		m_pPS->Release();
	}
	
	void Shader::Create(LPCWSTR source, ID3D11Device* device)
	{
		ID3D10Blob *VS, *PS;
		D3DCompileFromFile(source, 0, 0, "VShader", "vs_4_0", 0, 0, &VS, 0);
		D3DCompileFromFile(source, 0, 0, "PShader", "ps_4_0", 0, 0, &PS, 0);

		device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVS);
		device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPS);
	}

	void Shader::Bind(ID3D11DeviceContext * devcon)
	{
		devcon->VSSetShader(m_pVS, 0, 0);
		devcon->PSSetShader(m_pPS, 0, 0);
	}
}