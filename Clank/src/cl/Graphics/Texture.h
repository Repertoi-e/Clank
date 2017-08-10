#pragma once

#include "cl/stdafx.h"

#include <d3d11.h>

namespace cl {

	enum class TextureFilter
	{
		LINEAR,
		NEAREST
	};

	enum class API PixelFormat
	{
		RGB,
		RGBA,
		LUMINANCE,
		LUMINANCE_ALPHA
	};

	struct API TextureDesc
	{
		PixelFormat Format;
		TextureFilter Filter;
	};

	struct API TextureLoadProperties
	{
		bool FlipHorizontal, FlipVertical;

		TextureLoadProperties()
			: FlipHorizontal(false), FlipVertical(false)
		{
		}

		TextureLoadProperties(bool flipHorizontal, bool flipVertical)
			: FlipHorizontal(flipHorizontal), FlipVertical(flipVertical)
		{
		}
	};

	class API Texture
	{
	protected:
		ID3D11ShaderResourceView* m_Handle;
		
		D3D11_TEXTURE2D_DESC m_Desc;
		ID3D11Texture2D* m_Texture;
		
		D3D11_SAMPLER_DESC m_SamplerDesc;
		ID3D11SamplerState* m_SamplerState;

		u32 m_Width, m_Height, m_BPP;
	public:
		Texture();
		~Texture();

		void Bind(u32 slot = 0);
		void Unbind(u32 slot = 0);

		inline ID3D11ShaderResourceView* GetHandle() { return m_Handle; }
	public:
		static DXGI_FORMAT PixelFormatToD3D(PixelFormat format);
	public:
		static void CreateFromFile(Texture* texture, const String& file, TextureDesc& textureDesc, TextureLoadProperties loadProperties = { 0, 0 });
	};
}