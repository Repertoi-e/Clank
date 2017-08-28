#include "cl/stdafx.h"
#include "Texture.h"

#include "Context.h"

#include "cl/Utils/FileUtils.h"

namespace cl {
	
	Texture::Texture(void)
		: m_LoadProperties(L"NULL")
	{
	}
	
	Texture::~Texture(void)
	{
		SafeRelease(m_Handle);
		SafeRelease(m_Texture);
		SafeRelease(m_SamplerState);
	}

	void Texture::Load(void)
	{
		byte* data = NULLPTR;
		
		if (m_LoadProperties.File != L"NULL")
		{
			data = LoadImage(m_LoadProperties.File, &m_Desc.Width, &m_Desc.Height, &m_Desc.BPP, m_LoadProperties.FlipHorizontal, !m_LoadProperties.FlipVertical);
			m_Desc.Format = m_Desc.BPP == 24 ? PixelFormat::RGB : PixelFormat::RGBA;
		}
		
		bool generateMips = data != NULLPTR;

		D3D11_SUBRESOURCE_DATA initData;
		{
			ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));

			initData.pSysMem = data;
			initData.SysMemPitch = 4 * m_Desc.Width;
			initData.SysMemSlicePitch = m_Desc.Width * m_Desc.Height * 4;
		}

		u32 mipLevels = 1;
		if (generateMips)
		{
			u32 width = m_Desc.Width;
			u32 height = m_Desc.Height;

			while (width > 1 && height > 1)
			{
				width = std::max(width / 2, 1u);
				height = std::max(height / 2, 1u);
				++mipLevels;
			}
		}

		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

		u32 fmtSupport = 0;
		Context::Instance().GetDevice()->CheckFormatSupport(format, &fmtSupport);
		ASSERT(fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN, "");

		D3D11_TEXTURE2D_DESC desc;
		{
			ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

			desc.Width = m_Desc.Width;
			desc.Height = m_Desc.Height;
			desc.MipLevels = mipLevels;
			desc.ArraySize = 1;
			desc.Format = format;
			desc.CPUAccessFlags = 0;
			desc.Usage = generateMips ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
			desc.CPUAccessFlags = desc.Usage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			if (generateMips)
				desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			desc.MiscFlags = generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
		}
		HR(Context::Instance().GetDevice()->CreateTexture2D(&desc, NULLPTR, &m_Texture));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		{
			ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

			srvDesc.Format = desc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
		}
		HR(Context::Instance().GetDevice()->CreateShaderResourceView(m_Texture, &srvDesc, &m_Handle));

		if (generateMips)
		{
			Context::Instance().GetDeviceContext()->UpdateSubresource(m_Texture, 0, NULLPTR, initData.pSysMem, initData.SysMemPitch, initData.SysMemSlicePitch);
			Context::Instance().GetDeviceContext()->GenerateMips(m_Handle);
		}

		D3D11_SAMPLER_DESC samplerDesc;
		{
			ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = 11;
			samplerDesc.Filter = m_Desc.Filter == TextureFilter::LINEAR ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		}
		HR(Context::Instance().GetDevice()->CreateSamplerState(&samplerDesc, &m_SamplerState));

		if (data)
			del[] data;
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

	void Texture::SetData(const byte* data)
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		{
			ZeroMemory(&msr, sizeof(D3D11_MAPPED_SUBRESOURCE));
		}
		HR(Context::Instance().GetDeviceContext()->Map(m_Texture, NULLPTR, D3D11_MAP_WRITE_DISCARD, NULLPTR, &msr));
		
		u32 stride = m_Desc.Format == PixelFormat::RGB ? 3 : 4;
		for (u32 i = 0; i < m_Desc.Width * m_Desc.Height * stride; i += 4)
		{
			(cast(byte*) msr.pData)[i + 0] = 0xff;
			(cast(byte*) msr.pData)[i + 1] = 0xff;
			(cast(byte*) msr.pData)[i + 2] = 0xff;
			(cast(byte*) msr.pData)[i + 3] = (cast(byte*) data)[i / 2 + 1];
		}
		Context::Instance().GetDeviceContext()->Unmap(m_Texture, NULLPTR);
	}

	Texture* Texture::CreateFromFile(Texture* texture, const FileLoadProperties& loadProperties, const TextureDesc& textureDesc)
	{
		texture->m_LoadProperties = loadProperties;
		texture->m_Desc = textureDesc;
		texture->Load();

		return texture;
	}

	Texture* Texture::CreateFromMemory(Texture* texture, const TextureDesc& desc, const byte* data)
	{
		texture->m_Desc = desc;
		texture->Load();

		texture->SetData(data);

		return texture;
	}
}