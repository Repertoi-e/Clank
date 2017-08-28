#pragma once

#include "cl/stdafx.h"

#include "cl/Maths/maths.h"
#include "cl/Maths2/Rectangle.h"
#include "cl/Graphics/Texture.h"

#include "FontTexture.h"
#include "FontTextureAtlas.h"


namespace cl {

	class API Font
	{
	private:
		//FontTexture* m_FontTexture;
		//FontTextureAtlas* m_Atlas;
		texture_font_t* m_FontTexture;
		texture_atlas_t* m_Atlas;
		float32 m_Size;
		vec2 m_Scale;
		String m_Name;
		String m_Filename;
		mutable Texture* m_Texture;
	public:
		Font(const String& name, const byte* data, u32 datasize, float32 size);

		void SetTexture(void);

		void SetScale(float32 x, float32 y);

		inline void SetScale(const vec2& scale) { m_Scale = scale; }
		inline const vec2& GetScale() const { return m_Scale; }

		inline texture_font_t* GetFTFont() const { return m_FontTexture; }
		inline const String& GetName() const { return m_Name; }
		inline const String& GetFileName() const { return m_Filename; }
		inline float GetFontSize() const { return m_Size; }

		vec2 GetOffsets(const String& text) const;
		float32 GetWidth(const String& text) const;
		float32 GetHeight(const String& text) const;
		vec2 GetSize(const String& text) const;

		Rectangle GetBounds(const String& text) const;

		Texture* GetTexture() const;
	private:
		void UpdateAtlas() const;
	};
}