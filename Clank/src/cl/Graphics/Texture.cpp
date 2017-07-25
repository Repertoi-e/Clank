#include "cl/stdafx.h"
#include "Texture.h"

#include "Context.h"

#include <DDSTextureLoader.h>

namespace cl {
	
	Texture::Texture()
		: m_Handle(NULLPTR)
	{
	}
	
	Texture::~Texture()
	{
		if (m_Handle)
		{
			m_Handle->Release();
			m_Handle = 0;
		}
	}
	
	void Texture::CreateFromFile(String file)
	{
		DirectX::CreateDDSTextureFromFile(Context::Instance().GetDevice(), file.c_str(), NULLPTR, &m_Handle);

		ZeroMemory(&m_SamplerDesc, sizeof(D3D11_SAMPLER_DESC));

		m_SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		m_SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		m_SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		m_SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		m_SamplerDesc.MipLODBias = 0.0f;
		m_SamplerDesc.MaxAnisotropy = 1;
		m_SamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		m_SamplerDesc.BorderColor[0] = 0;
		m_SamplerDesc.BorderColor[1] = 0;
		m_SamplerDesc.BorderColor[2] = 0;
		m_SamplerDesc.BorderColor[3] = 0;
		m_SamplerDesc.MinLOD = 0;
		m_SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		
		HR(Context::Instance().GetDevice()->CreateSamplerState(&m_SamplerDesc, &m_SamplerState));
	}

	void Texture::Bind(u32 slot)
	{
		Context::Instance().GetDeviceContext()->PSSetShaderResources(slot, 1, &m_Handle);
		Context::Instance().GetDeviceContext()->PSSetSamplers(slot, 1, &m_SamplerState);
	}
	
	void Texture::Unbind(u32 slot)
	{
		ID3D11ShaderResourceView* rv = NULLPTR;
		Context::Instance().GetDeviceContext()->PSSetShaderResources(slot, 1, &rv);
	}
}