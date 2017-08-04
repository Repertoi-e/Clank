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


	struct API TextureSettings
	{
		PixelFormat Format;
		TextureFilter Filter;
	};

	struct API TextureLoadSettings
	{
		bool FlipHorizontal, FlipVertical;

		TextureLoadSettings()
			: FlipHorizontal(false), FlipVertical(false)
		{
		}

		TextureLoadSettings(bool flipHorizontal, bool flipVertical)
			: FlipHorizontal(flipHorizontal), FlipVertical(flipVertical)
		{
		}
	};

	class API Texture
	{
	protected:
		ID3D11ShaderResourceView* m_Handle;
		ID3D11Texture2D* m_Texture;
		ID3D11SamplerState* m_SamplerState;
		D3D11_TEXTURE2D_DESC m_Desc;
		D3D11_SAMPLER_DESC m_SamplerDesc;

		TextureSettings m_Settings;
		TextureLoadSettings m_LoadSettings;

		u32 m_Width, m_Height, m_BPP;
	public:
		Texture(TextureSettings& settings, TextureLoadSettings& loadSettings);
		~Texture();

		// @Incomplete: Currently there is no way to set the data of the texture
		// without loading it from a file.
		void CreateFromFile(String file);
		
		void Bind(u32 slot = 0);
		void Unbind(u32 slot = 0);

		inline ID3D11ShaderResourceView* GetHandle() { return m_Handle; }

		static DXGI_FORMAT PixelFormatToD3D(PixelFormat format);
	};
}