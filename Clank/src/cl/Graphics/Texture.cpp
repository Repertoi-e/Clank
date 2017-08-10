#include "cl/stdafx.h"
#include "Texture.h"

#include "Context.h"

#include "cl/Utils/LoadImage.h"

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

	void Texture::CreateFromFile(Texture* texture, const String& file, TextureDesc& textureDesc, TextureLoadProperties loadProperties)
	{
		byte* data = nullptr;
		data = LoadImage(file, &texture->m_Width, &texture->m_Height, &texture->m_BPP, loadProperties.FlipHorizontal, !loadProperties.FlipVertical);
		textureDesc.Format = texture->m_BPP == 24 ? PixelFormat::RGB : PixelFormat::RGBA;

		bool generateMips = data != nullptr;

		u32 stride = 4;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = data;
		initData.SysMemPitch = stride * texture->m_Width;
		initData.SysMemSlicePitch = texture->m_Width * texture->m_Height * stride;

		D3D11_SUBRESOURCE_DATA* initDataPtr = nullptr;
		u32 miplevels = 1;

		if (generateMips)
		{
			u32 width = texture->m_Width;
			u32 height = texture->m_Height;

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

		DXGI_FORMAT format = PixelFormatToD3D(textureDesc.Format);

		u32 fmtSupport = 0;
		Context::Instance().GetDevice()->CheckFormatSupport(format, &fmtSupport);
		ASSERT(fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN, "");

		{
			ZeroMemory(&texture->m_Desc, sizeof(D3D11_TEXTURE2D_DESC));

			texture->m_Desc.Width = texture->m_Width;
			texture->m_Desc.Height = texture->m_Height;
			texture->m_Desc.MipLevels = miplevels;
			texture->m_Desc.ArraySize = 1;
			texture->m_Desc.Format = format;
			texture->m_Desc.CPUAccessFlags = 0;
			texture->m_Desc.Usage = generateMips ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
			texture->m_Desc.CPUAccessFlags = texture->m_Desc.Usage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;
			texture->m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			if (generateMips)
				texture->m_Desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			texture->m_Desc.MiscFlags = generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
			texture->m_Desc.SampleDesc.Count = 1;
			texture->m_Desc.SampleDesc.Quality = 0;
		}
		HR(Context::Instance().GetDevice()->CreateTexture2D(&texture->m_Desc, initDataPtr, &texture->m_Texture));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		{
			ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

			srvDesc.Format = texture->m_Desc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = texture->m_Desc.MipLevels;
		}
		HR(Context::Instance().GetDevice()->CreateShaderResourceView(texture->m_Texture, &srvDesc, &texture->m_Handle));

		if (generateMips)
		{
			Context::Instance().GetDeviceContext()->UpdateSubresource(texture->m_Texture, 0, nullptr, initData.pSysMem, initData.SysMemPitch, initData.SysMemSlicePitch);
			Context::Instance().GetDeviceContext()->GenerateMips(texture->m_Handle);
		}

		{
			ZeroMemory(&texture->m_SamplerDesc, sizeof(D3D11_SAMPLER_DESC));

			texture->m_SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			texture->m_SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			texture->m_SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			texture->m_SamplerDesc.MinLOD = 0;
			texture->m_SamplerDesc.MaxLOD = 11;
			texture->m_SamplerDesc.Filter = textureDesc.Filter == TextureFilter::LINEAR ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
			texture->m_SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			texture->m_SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		}
		HR(Context::Instance().GetDevice()->CreateSamplerState(&texture->m_SamplerDesc, &texture->m_SamplerState));

		if (data != nullptr)
			cl_delete[] data;
	}
}