#include "cl/stdafx.h"
#include "FontTexture.h"

#include <assert.h>

#include "cl/Utils/Log.h"

namespace cl {

	TextureGlyph::~TextureGlyph()
	{
		del kerning;
	}

	float32 TextureGlyph::GetKerning(const wchar charcode)
	{
		for (u32 i = 0; i < kerning->Size(); ++i)
		{
			Kerning* kerning = cast(Kerning*) this->kerning->Get(i);
			if (kerning->charcode == charcode)
				return kerning->kerning;
		}
		return 0;
	}

	void FontTexture::GenerateKerning(void)
	{
		FT_Library library;
		FT_Face face;
		if (!GetFace(&library, &face))
			return;

		TextureGlyph* glyph;
		TextureGlyph* prev_glyph;

		u32 glyph_index, prev_index;
		FT_Vector kerning;

		for (u32 i = 1; i < m_Glyphs->Size(); ++i)
		{
			glyph = *cast(TextureGlyph**) m_Glyphs->Get(i);
			glyph_index = FT_Get_Char_Index(face, glyph->charcode);
			glyph->kerning->Clear();

			for (u32 j = 1; j < m_Glyphs->Size(); ++j)
			{
				prev_glyph = *cast(TextureGlyph**)m_Glyphs->Get(j);
				prev_index = FT_Get_Char_Index(face, prev_glyph->charcode);
				FT_Get_Kerning(face, prev_index, glyph_index, FT_KERNING_UNFITTED, &kerning);
				if (kerning.x)
				{
					Kerning k = { prev_glyph->charcode, kerning.x / (float32)(HRESf*HRESf) };
					glyph->kerning->PushBack(&k);
				}
			}
		}

		FT_Done_Face(face);
		FT_Done_FreeType(library);
	}

	FontTexture::FontTexture(FontTextureAtlas* atlas, float32 size, const void* memory_base, u32 memory_size)
		: m_Atlas(atlas), m_Size(size)
	{
		ASSERT(memory_base, "Memory is NULLPTR");
		ASSERT(memory_size, "Memory size is 0");

		m_Memory.base = memory_base;
		m_Memory.size = memory_size;

		if (Init())
		{
			LOG_ERROR("Failed init");
		}
	}

	FontTexture::~FontTexture()
	{
		TextureGlyph* glyph;

		for (u32 i = 0; i < m_Glyphs->Size(); ++i)
		{
			glyph = *cast(TextureGlyph**)m_Glyphs->Get(i);
			del glyph;
		}

		del m_Glyphs;
	}

	s32 FontTexture::Init(void)
	{
		// FT_LCD_FILTER_LIGHT   is (0x00, 0x55, 0x56, 0x55, 0x00)
		// FT_LCD_FILTER_DEFAULT is (0x10, 0x40, 0x70, 0x40, 0x10)
		m_LCDWeights[0] = 0x10;
		m_LCDWeights[1] = 0x40;
		m_LCDWeights[2] = 0x70;
		m_LCDWeights[3] = 0x40;
		m_LCDWeights[4] = 0x10;

		FT_Library library;
		FT_Face face; 
		if (!GetHiresFace(&library, &face))
			return -1;

		m_UnderlinePosition = face->underline_position / (float32)(HRESf * HRESf) * m_Size;
		m_UnderlinePosition = round(m_UnderlinePosition);
		if (m_UnderlinePosition > -2)
		{
			m_UnderlinePosition = -2.0;
		}

		m_UnderlineThickness = face->underline_thickness / (float32)(HRESf * HRESf) * m_Size;
		m_UnderlineThickness = round(m_UnderlineThickness);
		if (m_UnderlineThickness < 1)
		{
			m_UnderlineThickness = 1.0;
		}

		FT_Size_Metrics metrics = face->size->metrics;
		m_Ascender = (metrics.ascender >> 6) / 100.0f;
		m_Descender = (metrics.descender >> 6) / 100.0f;
		m_Height = (metrics.height >> 6) / 100.0f;
		m_LineGap = m_Height - m_Ascender + m_Descender;
		FT_Done_Face(face);
		FT_Done_FreeType(library);

		GetGlyph(-1);

		return 0;
	}

	u32 FontTexture::LoadGlyphs(const wchar* charcodes)
	{
		u32 missed = 0;

		ASSERT(charcodes, "No charcodes");

		u32 width = m_Atlas->m_Width;
		u32 height = m_Atlas->m_Height;
		u32 depth = m_Atlas->m_Depth;

		if (depth == 2)
			depth = 1;

		FT_Library library;
		FT_Face face;
		if (!GetFace(&library, &face))
			return wcslen(charcodes);

		FT_Glyph ft_glyph = NULL;
		FT_Bitmap ft_bitmap;
		FT_GlyphSlot ft_slot;
		
		TextureGlyph* glyph;
		u32 glyph_index;
		
		s32 flags = 0;
		s32 ft_glyph_top = 0;
		s32 ft_glyph_left = 0;
		
		char pass;
		for (u32 i = 0; i < wcslen(charcodes); ++i)
		{
			pass = 0;
			for (u32 j = 0; j < m_Glyphs->Size(); ++j) 
			{
				glyph = *cast(TextureGlyph**) m_Glyphs->Get(j);
				if ((glyph->charcode == charcodes[i]) &&
					((charcodes[i] == cast(wchar) (-1)) ||
					((glyph->outline_type == m_OutlineType) &&
						(glyph->outline_thickness == m_OutlineThickness))))
				{
					pass = 1;
					break;
				}
			}

			if (pass)
				continue; 

			flags = 0;
			ft_glyph_top = 0;
			ft_glyph_left = 0;
			glyph_index = FT_Get_Char_Index(face, charcodes[i]);

			flags |= m_OutlineType > 0 ? FT_LOAD_NO_BITMAP : FT_LOAD_RENDER;
			flags |= m_Hinting ? FT_LOAD_FORCE_AUTOHINT : FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT;

			if (depth == 3)
			{
				FT_Library_SetLcdFilter(library, FT_LCD_FILTER_LIGHT);
				flags |= FT_LOAD_TARGET_LCD;
				if (m_Filtering)
					FT_Library_SetLcdFilterWeights(library, m_LCDWeights);
			}
			
			FT_Error error = FT_Load_Glyph(face, glyph_index, flags);
			if (error)
			{
				LOG_ERROR("FT_Error (", FT_Errors[error].code, " : ", FT_Errors[error].message, "\n");
				FT_Done_Face(face);
				FT_Done_FreeType(library);
				return wcslen(charcodes) - i;
			}


			if (m_OutlineType == 0)
			{
				ft_slot = face->glyph;
				ft_bitmap = ft_slot->bitmap;
				ft_glyph_top = ft_slot->bitmap_top;
				ft_glyph_left = ft_slot->bitmap_left;
			}
			else
			{
				FT_Stroker stroker;
				FT_BitmapGlyph ft_bitmap_glyph;
				error = FT_Stroker_New(library, &stroker);
				if (error)
				{
					LOG_ERROR("FT_Error (", FT_Errors[error].code, " : ", FT_Errors[error].message, "\n");
					FT_Done_Face(face);
					FT_Stroker_Done(stroker);
					FT_Done_FreeType(library);
					return 0;
				}
				FT_Stroker_Set(stroker, cast(s32) (m_OutlineThickness * HRES), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
				
				error = FT_Get_Glyph(face->glyph, &ft_glyph);
				if (error)
				{
					LOG_ERROR("FT_Error (", FT_Errors[error].code, " : ", FT_Errors[error].message, "\n");
					FT_Done_Face(face);
					FT_Stroker_Done(stroker);
					FT_Done_FreeType(library);
					return 0;
				}

				if (m_OutlineType == 1)
					error = FT_Glyph_Stroke(&ft_glyph, stroker, 1);
				else if (m_OutlineType == 2)
					error = FT_Glyph_StrokeBorder(&ft_glyph, stroker, 0, 1);
				else if (m_OutlineType == 3)
					error = FT_Glyph_StrokeBorder(&ft_glyph, stroker, 1, 1);
				
				if (error)
				{
					LOG_ERROR("FT_Error (", FT_Errors[error].code, " : ", FT_Errors[error].message, "\n");
					FT_Done_Face(face);
					FT_Stroker_Done(stroker);
					FT_Done_FreeType(library);
					return 0;
				}

				if (depth == 1)
				{
					error = FT_Glyph_To_Bitmap(&ft_glyph, FT_RENDER_MODE_NORMAL, 0, 1);
					if (error)
					{
						LOG_ERROR("FT_Error (", FT_Errors[error].code, " : ", FT_Errors[error].message, "\n");
						FT_Done_Face(face);
						FT_Stroker_Done(stroker);
						FT_Done_FreeType(library);
						return 0;
					}
				}
				else
				{
					error = FT_Glyph_To_Bitmap(&ft_glyph, FT_RENDER_MODE_LCD, 0, 1);
					if (error)
					{
						LOG_ERROR("FT_Error (", FT_Errors[error].code, " : ", FT_Errors[error].message, "\n");
						FT_Done_Face(face);
						FT_Stroker_Done(stroker);
						FT_Done_FreeType(library);
						return 0;
					}
				}
				ft_bitmap_glyph = cast(FT_BitmapGlyph) ft_glyph;
				ft_bitmap = ft_bitmap_glyph->bitmap;
				ft_glyph_top = ft_bitmap_glyph->top;
				ft_glyph_left = ft_bitmap_glyph->left;
				FT_Stroker_Done(stroker);
			}

			u32 w = ft_bitmap.width / depth + 1;
			u32 h = ft_bitmap.rows + 1;
			
			vec4i region = m_Atlas->GetRegion(w, h);
			if (region.x < 0)
			{
				missed++;
				LOG_ERROR("Texture atlas is full!\n");
				continue;
			}
			
			w = w - 1;
			h = h - 1;

			u32 x = region.x;
			u32 y = region.y;
			m_Atlas->SetRegion(x, y, w, h, ft_bitmap.buffer, ft_bitmap.pitch);

			glyph = anew TextureGlyph;
			glyph->charcode = charcodes[i];
			glyph->width = w;
			glyph->height = h;
			glyph->outline_type = m_OutlineType;
			glyph->outline_thickness = m_OutlineThickness;
			glyph->offset_x = ft_glyph_left;
			glyph->offset_y = ft_glyph_top;
			glyph->s0 = x / cast(float32) width;
			glyph->t0 = y / cast(float32) height;
			glyph->s1 = (x + glyph->width) / cast(float32) width;
			glyph->t1 = (y + glyph->height) / cast(float32) height;

			FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
			ft_slot = face->glyph;
			glyph->advance_x = ft_slot->advance.x / HRESf;
			glyph->advance_y = ft_slot->advance.y / HRESf;

			m_Glyphs->PushBack(&glyph);

			if (m_OutlineType > 0)
				FT_Done_Glyph(ft_glyph);
		}
		
		m_Atlas->SetDirty(1);
		
		GenerateKerning();
		
		FT_Done_Face(face);
		FT_Done_FreeType(library);
	
		return missed;
	}

	TextureGlyph* FontTexture::GetGlyph(wchar charcode)
	{
		wchar buffer[2] = { 0,0 };
		TextureGlyph* glyph;

		for (u32 i = 0; i<m_Glyphs->Size(); ++i)
		{
			glyph = *cast(TextureGlyph**) m_Glyphs->Get(i);
			if ((glyph->charcode == charcode) &&
				((charcode == (wchar)(-1)) ||
				((glyph->outline_type == m_OutlineType) &&
					(glyph->outline_thickness == m_OutlineThickness))))
			{
				return glyph;
			}
		}

		if (charcode == (wchar)(-1))
		{
			u32 width = m_Atlas->m_Width;
			u32 height = m_Atlas->m_Height;
			vec4i region = m_Atlas->GetRegion(5, 5);
			
			byte i = -1;
			static byte data[] = {  i, i, i, i, i, i, i, i, i, i, i, i,
				 i, i, i, i, i, i, i, i, i, i, i, i,
				 i, i, i, i, i, i, i, i, i, i, i, i,
				 i, i, i, i, i, i, i, i, i, i, i, i };
			if (region.x < 0)
			{
				LOG_ERROR("Texture atlas is full!\n");
				return NULL;
			}

			m_Atlas->SetRegion(region.x, region.y, 4, 4, data, 0);
			
			TextureGlyph* glyph = anew TextureGlyph;
			glyph->charcode = cast(wchar) (-1);
			glyph->s0 = (region.x + 2) / cast(float32) width;
			glyph->t0 = (region.y + 2) / cast(float32) height;
			glyph->s1 = (region.x + 3) / cast(float32) width;
			glyph->t1 = (region.y + 3) / cast(float32) height;
			m_Glyphs->PushBack(&glyph);
			return glyph;
		}

		buffer[0] = charcode;
		if (LoadGlyphs(buffer) == 0)
			return *cast(TextureGlyph**)m_Glyphs->Back();
		
		return NULL;
	}

	s32 FontTexture::LoadFace(float32 size, FT_Library* library, FT_Face* face)
	{
		FT_Matrix matrix = {
			(s32)((1.0 / HRES) * 0x10000L),
			(s32)((0.0) * 0x10000L),
			(s32)((0.0) * 0x10000L),
			(s32)((1.0) * 0x10000L) };

		ASSERT(library, "FT_Library is NULLPTR");
		ASSERT(size, "Size is 0");

		FT_Error error = FT_Init_FreeType(library);
		if (error) 
		{
			LOG_ERROR("FT_Error (", FT_Errors[error].code, " : ", FT_Errors[error].message, "\n");
			return 0;
		}

		error = FT_New_Memory_Face(*library, cast(FT_Byte*) m_Memory.base, m_Memory.size, 0, face);

		if (error) 
		{
			LOG_ERROR("FT_Error (", FT_Errors[error].code, " : ", FT_Errors[error].message, "\n");
			FT_Done_FreeType(*library);
			return 0;
		}

		error = FT_Select_Charmap(*face, FT_ENCODING_UNICODE);
		if (error) 
		{
			LOG_ERROR("FT_Error (", FT_Errors[error].code, " : ", FT_Errors[error].message, "\n");
			FT_Done_Face(*face);
			FT_Done_FreeType(*library);
			return 0;
		}

		error = FT_Set_Char_Size(*face, (s32)(size * HRES), 0, DPI * HRES, DPI);

		if (error) 
		{
			LOG_ERROR("FT_Error (", FT_Errors[error].code, " : ", FT_Errors[error].message, "\n");
			FT_Done_Face(*face);
			FT_Done_FreeType(*library);
			return 0;
		}

		FT_Set_Transform(*face, &matrix, NULL);

		return 1;
	}

	s32 FontTexture::GetFace(FT_Library* library, FT_Face* face)
	{
		return GetFace(m_Size, library, face);
	}

	s32 FontTexture::GetFace(float32 size, FT_Library* library, FT_Face* face)
	{
		return LoadFace(size, library, face);
	}

	s32 FontTexture::GetHiresFace(FT_Library* library, FT_Face* face)
	{
		return LoadFace(m_Size * 100.f, library, face);
	}
}
