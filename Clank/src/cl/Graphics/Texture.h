#pragma once

#include "cl/stdafx.h"

#include <d3d11.h>

namespace cl {

	enum class TextureFilter
	{
		LINEAR,
		NEAREST
	};

	enum class PixelFormat
	{
		RGB,
		RGBA
	};

	struct TextureDesc
	{
		String Name;
		TextureFilter Filter;
		PixelFormat Format;
		u32 Width;
		u32 Height;
		u32 BPP;
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
		ID3D11Texture2D* m_Texture = NULLPTR;
		ID3D11SamplerState* m_SamplerState = NULLPTR;

		FileLoadProperties m_LoadProperties;
		TextureDesc m_Desc;
	public:
		Texture(void);
		~Texture(void);

		void Load(void);

		void Bind(u32 slot = 0);
		void Unbind(u32 slot = 0);

		void SetData(const byte* data);

		inline const String& GetName() const { return m_Desc.Name; }
		inline const TextureDesc& GetDesc() const { return m_Desc; }

		inline ID3D11ShaderResourceView* GetHandle(void) { return m_Handle; }
	public:
		static Texture* CreateFromFile(Texture* texture, const FileLoadProperties& loadProperties, const TextureDesc& textureDesc);
		static Texture* CreateFromMemory(Texture* texture, const TextureDesc& desc, const byte* data);
	};
}