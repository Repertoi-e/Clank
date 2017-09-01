#pragma once

#include "cl/stdafx.h"

#include "FontTextureAtlas.h"

#define HRES  64
#define HRESf 64.f
#define DPI   72

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftstroke.h>
#include <freetype/ftlcdfil.h>

#include "FTErrors.h"

namespace cl {

	struct Kerning
	{
		wchar charcode;
		float32 kerning;
	};

	struct TextureGlyph
	{
		wchar charcode;
		u32 id = 0u;

		u32 width = 0u;
		u32 height = 0u;

		s32 offset_x = 0;
		s32 offset_y = 0;

		float32 advance_x = 0.0f;
		float32 advance_y = 0.0f;

		float32 s0 = 0.0f;
		float32 t0 = 0.0f;
		float32 s1 = 0.0f;
		float32 t1 = 0.0f;

		Vector* kerning = anew Vector(sizeof(Kerning));

		s32 outline_type = 0;
		float32 outline_thickness = 0.0f;
	public:
		~TextureGlyph();

		float32 GetKerning(const wchar charcode);
	};

	class FontTexture
	{
	private:
		Vector* m_Glyphs = anew Vector(sizeof(TextureGlyph*));
		FontTextureAtlas* m_Atlas;
		
		struct Memory
		{
			const void*base;
			u32 size;
		};
		Memory m_Memory;

		s32 m_Hinting = 1, m_OutlineType = 0, m_Filtering = 1, m_Kerning = 1;

		byte m_LCDWeights[5];

		float32 m_Size, m_Height = 0.0f, m_LineGap;
		float32 m_Ascender = 0.0f, m_Descender = 0.0f;
		float32 m_OutlineThickness = 0.0f, m_UnderlinePosition = 0.0f, m_UnderlineThickness = 0.0f;
	public:
		FontTexture(FontTextureAtlas* atlas, float32 size, const void* memory_base, u32 memory_size);
		~FontTexture();

		u32 LoadGlyphs(const wchar* charcodes);

		TextureGlyph* GetGlyph(wchar charcode);
	private:
		s32 Init(void);

		void GenerateKerning(void);

		s32 LoadFace(float32 size, FT_Library* library, FT_Face* face);

		s32 GetFace(FT_Library* library, FT_Face* face);
		s32 GetFace(float32 size, FT_Library* library, FT_Face* face);
		s32 GetHiresFace(FT_Library* library, FT_Face* face);
	};
}