#include "cl/stdafx.h"
#include "Font.h"

#include "cl/Utils/FileUtils.h"
#include "cl/Utils/Log.h"

namespace cl {

	Font::Font()
	{
	}

	Font::~Font()
	{
		del m_Texture;
		del m_FontTexture;
		del m_FontTextureAtlas;
	}

	Texture* Font::GetTexture() const
	{
		UpdateAtlas();
		return m_Texture;
	}

	void Font::UpdateAtlas() const
	{
		if (m_FontTextureAtlas->m_Dirty)
		{
			m_Texture->SetData(cast(const byte*) m_FontTextureAtlas->m_Data);
			m_FontTextureAtlas->m_Dirty = false;
		}
	}

	vec2 Font::GetOffsets(const String& text) const
	{
		if (text.empty())
			return vec2(0.0f, 0.0f);

		TextureGlyph* glyph = m_FontTexture->GetGlyph(text[0]);
		ASSERT(glyph, "");

		float32 yo = 0.0f;
		const vec2& scale = m_Desc.Scale;
		for (u32 i = 0; i < text.size(); i++)
		{
			TextureGlyph* glyph = m_FontTexture->GetGlyph(text[i]);
			ASSERT(glyph, "");
			float32 height = glyph->height / scale.y;
			float32 offset = glyph->offset_y / scale.y - height;
			if (offset < yo)
				yo = offset;
		}

		return vec2(glyph->offset_x / scale.x, yo);
	}

	float32 Font::GetWidth(const String& text) const
	{
		float32 width = 0.0f;
		const vec2& scale = m_Desc.Scale;
		for (u32 i = 0; i < text.size(); i++)
		{
			TextureGlyph* glyph = m_FontTexture->GetGlyph(text[i]);
			ASSERT(glyph, "");
			if (i > 0)
			{
				float32 kerning = glyph->GetKerning(text[i - 1]);
				width += kerning / scale.x;
			}
			width += glyph->advance_x / scale.x;
		}
		return width;
	}

	float Font::GetHeight(const String& text) const
	{
		const vec2& scale = m_Desc.Scale;
		float32 min = 0.0f;
		float32 max = 0.0f;
		for (u32 i = 0; i < text.size(); i++)
		{
			TextureGlyph* glyph = m_FontTexture->GetGlyph(text[i]);
			ASSERT(glyph, "");
			float height = glyph->height / scale.y;
			float offset = glyph->offset_y / scale.y - height;
			if (offset < min)
				min = offset;
			if (height > max)
				max = height;
		}
		return abs(min) + abs(max);
	}

	vec2 Font::GetSize(const String& text) const
	{
		return vec2(GetWidth(text), GetHeight(text));
	}

	Rectangle Font::GetBounds(const String& text) const
	{
		return Rectangle(0.0f, 0.0f, GetWidth(text), GetHeight(text));
	}

	Font* Font::CreateFromFile(Font* font, const FontDesc& desc)
	{
		font->m_Desc = desc;
		
		u32 dataSize = 0;
		byte* fontData = ReadFile(font->m_Desc.FileName, &dataSize);

		font->m_FontTextureAtlas = anew FontTextureAtlas(512, 512, 2);
		font->m_FontTexture = anew FontTexture(font->m_FontTextureAtlas, font->m_Desc.Size, fontData, dataSize);

		TextureDesc textureDesc;
		{
			ZeroMemory(&textureDesc, sizeof(TextureDesc));

			textureDesc.Format = PixelFormat::RGBA;
			textureDesc.Filter = TextureFilter::NEAREST;
			textureDesc.Width = 512u;
			textureDesc.Height = 512u;
		}
		Texture::CreateFromMemory(font->m_Texture, textureDesc, font->m_FontTextureAtlas->m_Data);

		return font;
	}
}