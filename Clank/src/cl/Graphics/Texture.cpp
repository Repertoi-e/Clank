#include "cl/stdafx.h"
#include "Texture.h"

#include "Context.h"

#include "cl/Utils/LoadImage.h"

namespace cl {
	
	Texture::Texture(TextureSettings& settings, TextureLoadSettings& loadSettings)
		: m_Handle(NULLPTR), m_Settings(settings), m_LoadSettings(loadSettings)
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
		byte* data = nullptr;
		data = LoadImage(file, &m_Width, &m_Height, &m_BPP, m_LoadSettings.FlipHorizontal, !m_LoadSettings.FlipVertical);
		m_Settings.Format = m_BPP == 24 ? PixelFormat::RGB : PixelFormat::RGBA;

		bool generateMips = data != nullptr;

		u32 stride = 4;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = data;
		initData.SysMemPitch = stride * m_Width;
		initData.SysMemSlicePitch = m_Width * m_Height * stride;

		D3D11_SUBRESOURCE_DATA* initDataPtr = nullptr;
		u32 miplevels = 1;

		if (generateMips)
		{
			u32 width = m_Width;
			u32 height = m_Height;

			while (width > 1 && height > 1)
			{
				width = std::max(width / 2, 1u);
				height = std::max(height / 2, 1u);
				++miplevels;
			}
		}
		else
			if (data) 
				initDataPtr = &initData;

		DXGI_FORMAT format = PixelFormatToD3D(m_Settings.Format);

		u32 fmtSupport = 0;
		Context::Instance().GetDevice()->CheckFormatSupport(format, &fmtSupport);
		ASSERT(fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN, "");

		{
			ZeroMemory(&m_Desc, sizeof(D3D11_TEXTURE2D_DESC));
			
			m_Desc.Width = m_Width;
			m_Desc.Height = m_Height;
			m_Desc.MipLevels = miplevels;
			m_Desc.ArraySize = 1;
			m_Desc.Format = format;
			m_Desc.CPUAccessFlags = 0;
			m_Desc.Usage = generateMips ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
			m_Desc.CPUAccessFlags = m_Desc.Usage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;
			m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			if (generateMips)
				m_Desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			m_Desc.MiscFlags = generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
			m_Desc.SampleDesc.Count = 1;
			m_Desc.SampleDesc.Quality = 0;
		}
		HR(Context::Instance().GetDevice()->CreateTexture2D(&m_Desc, initDataPtr, &m_Texture));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		{
			ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			
			srvDesc.Format = m_Desc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = m_Desc.MipLevels;
		}
		HR(Context::Instance().GetDevice()->CreateShaderResourceView(m_Texture, &srvDesc, &m_Handle));
		if (generateMips)
		{
			Context::Instance().GetDeviceContext()->UpdateSubresource(m_Texture, 0, nullptr, initData.pSysMem, initData.SysMemPitch, initData.SysMemSlicePitch);
			Context::Instance().GetDeviceContext()->GenerateMips(m_Handle);
		}

		{
			ZeroMemory(&m_SamplerDesc, sizeof(D3D11_SAMPLER_DESC));

			m_SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			m_SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			m_SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			m_SamplerDesc.MinLOD = 0;
			m_SamplerDesc.MaxLOD = 11;
			m_SamplerDesc.Filter = m_Settings.Filter == TextureFilter::LINEAR ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
			m_SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			m_SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		}
		HR(Context::Instance().GetDevice()->CreateSamplerState(&m_SamplerDesc, &m_SamplerState));

		if (data != nullptr)
			delete[] data;
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

	DXGI_FORMAT Texture::PixelFormatToD3D(PixelFormat format)
	{
		switch (format)
		{
			case PixelFormat::RGB:
			case PixelFormat::RGBA:
			case PixelFormat::LUMINANCE_ALPHA:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		return DXGI_FORMAT_UNKNOWN;
	}
}