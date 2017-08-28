#pragma once

#include "cl/stdafx.h"

#include "cl/Maths/maths.h"
#include "cl/Maths2/Rectangle.h"
#include "cl/Graphics/Texture.h"

#include "FontTexture.h"
#include "FontTextureAtlas.h"


namespace cl {

	struct API FontDesc
	{
		String Name, FileName;
		float32 Size;
		vec2 Scale;
	};

	class API Font
	{
	private:
		FontTexture* m_FontTexture = NULLPTR;
		FontTextureAtlas* m_FontTextureAtlas = NULLPTR;

		FontDesc m_Desc;

		Texture* m_Texture = anew Texture;
	public:
		Font();
		~Font();

		vec2 GetOffsets(const String& text) const;
		vec2 GetSize(const String& text) const;
		float32 GetWidth(const String& text) const;
		float32 GetHeight(const String& text) const;

		Rectangle GetBounds(const String& text) const;
		Texture* GetTexture() const;
	
		inline const String& GetName() const { return m_Desc.Name; }
		inline const FontDesc& GetDesc() const { return m_Desc; }
		inline FontTexture* GetFTFont() const { return m_FontTexture; }
	private:
		void UpdateAtlas() const;
	public:
		static Font* CreateFromFile(Font* font, const FontDesc& desc);
	};
}