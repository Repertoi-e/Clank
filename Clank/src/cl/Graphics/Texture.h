#pragma once

#include "cl/stdafx.h"

#include <d3d11.h>

namespace cl {

	enum class TextureFilter
	{
		LINEAR,
		NEAREST
	};

	struct TextureDesc
	{
		TextureFilter Filter;
	};

	struct API FileLoadProperties
	{
		String File;
		bool FlipHorizontal = false, FlipVertical = false;

		FileLoadProperties(String file)
			: File(file)
		{
		}

		FileLoadProperties(String file, bool flipHorizontal, bool flipVertical)
			: File(file), FlipHorizontal(flipHorizontal), FlipVertical(flipVertical)
		{
		}
	};

	class API Texture
	{
	protected:
		ID3D11ShaderResourceView* m_Handle = NULLPTR;
		
		D3D11_TEXTURE2D_DESC m_Desc;
		ID3D11Texture2D* m_Texture = NULLPTR;
		
		D3D11_SAMPLER_DESC m_SamplerDesc;
		ID3D11SamplerState* m_SamplerState = NULLPTR;

		u32 m_Width, m_Height, m_BPP;
	public:
		Texture(void);
		~Texture(void);

		void Bind(u32 slot = 0);
		void Unbind(u32 slot = 0);

		inline ID3D11ShaderResourceView* GetHandle(void) { return m_Handle; }
	public:
		static void CreateFromFile(Texture* texture, const FileLoadProperties& loadProperties, const TextureDesc& textureDesc);
		static void CreateFromMemory();
	};
}