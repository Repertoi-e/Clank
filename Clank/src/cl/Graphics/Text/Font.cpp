#include "cl/stdafx.h"
#include "Font.h"

#include "cl/Utils/Log.h"

namespace cl {

	Font::Font(const String& name, const byte* data, u32 datasize, float32 size)
		: m_Name(name), m_Filename(L"NULL"), m_Size(size), m_Scale(vec2(1.0f, 1.0f)), m_Texture(NULLPTR)
	{
		//m_Atlas = anew FontTextureAtlas(512, 512, 2);
		//m_FontTexture = anew FontTexture(m_Atlas, size, data, datasize);

		m_Atlas = texture_atlas_new(512, 512, 2);
		m_FontTexture = texture_font_new_from_memory(m_Atlas, size, data, datasize);
	}

	void Font::SetTexture(void)
	{
		TextureDesc desc;
		desc.Filter = TextureFilter::LINEAR;
		desc.Width = 512u;
		desc.Height = 512u;
		desc.Format = PixelFormat::RGBA;

		m_Texture = anew Texture(desc, m_Atlas->data);
	}

	void Font::SetScale(float32 x, float32 y)
	{
		m_Scale = vec2(x, y);
	}

	Texture* Font::GetTexture() const
	{
		UpdateAtlas();
		return m_Texture;
	}

	void Font::UpdateAtlas() const
	{
		if (m_Atlas->dirty)
		{
			m_Texture->SetData(cast(const byte*) m_Atlas->data);
			m_Atlas->dirty = false;
		}
	}

	vec2 Font::GetOffsets(const String& text) const
	{
		if (text.empty())
			return vec2(0.0f, 0.0f);

		//texture_glyph_t* glyph = m_FontTexture->GetGlyph(text[0]);
		texture_glyph_t* glyph = texture_font_get_glyph(m_FontTexture, text[0]);
		ASSERT(glyph, "");

		float32 yo = 0.0f;
		const vec2& scale = m_Scale;
		for (int i = 0; i < text.size(); i++)
		{
			//texture_glyph_t* glyph = m_FontTexture->GetGlyph(text[i]);
			texture_glyph_t* glyph = texture_font_get_glyph(m_FontTexture, text[i]);
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
		const vec2& scale = m_Scale;
		for (int i = 0; i < text.size(); i++)
		{
			//texture_glyph_t* glyph = m_FontTexture->GetGlyph(text[i]);
			texture_glyph_t* glyph = texture_font_get_glyph(m_FontTexture, text[i]); 
			ASSERT(glyph, "");
			if (i > 0)
			{
				//float32 kerning = glyph->GetKerning(text[i - 1]);
				float32 kerning = texture_glyph_get_kerning(glyph, text[i - 1]);
				width += kerning / scale.x;
			}
			width += glyph->advance_x / scale.x;
		}
		return width;
	}

	float Font::GetHeight(const String& text) const
	{
		const vec2& scale = m_Scale;
		float32 min = 0.0f;
		float32 max = 0.0f;
		for (int i = 0; i < text.size(); i++)
		{
			//texture_glyph_t* glyph = m_FontTexture->GetGlyph(text[i]);
			texture_glyph_t* glyph = texture_font_get_glyph(m_FontTexture, text[i]); 
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
}