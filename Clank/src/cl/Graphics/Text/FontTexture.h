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

#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { 0, 0 } };
const struct {
	s32          code;
	const char*  message;
} FT_Errors[] =
#ifndef __FTERRORS_H__
#define __FTERRORS_H__


#include FT_MODULE_ERRORS_H


#undef  FT_NEED_EXTERN_C

#ifndef FT_ERR_PREFIX
#define FT_ERR_PREFIX  FT_Err_
#endif


#ifdef FT_CONFIG_OPTION_USE_MODULE_ERRORS

#ifndef FT_ERR_BASE
#define FT_ERR_BASE  FT_Mod_Err_Base
#endif

#else

#undef FT_ERR_BASE
#define FT_ERR_BASE  0

#endif /* FT_CONFIG_OPTION_USE_MODULE_ERRORS */


#ifndef FT_ERRORDEF

#define FT_ERRORDEF( e, v, s )  e = v,
#define FT_ERROR_START_LIST     enum {
#define FT_ERROR_END_LIST       FT_ERR_CAT( FT_ERR_PREFIX, Max ) };

#ifdef __cplusplus
#define FT_NEED_EXTERN_C
extern "C" {
#endif

#endif /* !FT_ERRORDEF */


#define FT_ERRORDEF_( e, v, s )                                             \
          FT_ERRORDEF( FT_ERR_CAT( FT_ERR_PREFIX, e ), v + FT_ERR_BASE, s )

#define FT_NOERRORDEF_( e, v, s )                             \
          FT_ERRORDEF( FT_ERR_CAT( FT_ERR_PREFIX, e ), v, s )


#ifdef FT_ERROR_START_LIST
	FT_ERROR_START_LIST
#endif


#include FT_ERROR_DEFINITIONS_H


#ifdef FT_ERROR_END_LIST
		FT_ERROR_END_LIST
#endif

#ifdef FT_NEED_EXTERN_C
}
#endif

#undef FT_ERROR_START_LIST
#undef FT_ERROR_END_LIST

#undef FT_ERRORDEF
#undef FT_ERRORDEF_
#undef FT_NOERRORDEF_

#undef FT_NEED_EXTERN_C
#undef FT_ERR_BASE

#ifndef FT2_BUILD_LIBRARY
#undef FT_ERR_PREFIX
#endif

#endif /* __FTERRORS_H__ */

namespace cl {

	struct kerning_t
	{
		wchar charcode;
		float32 kerning;
	};

	struct texture_glyph_t
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

		Vector* kerning;

		s32 outline_type = 0;
		float32 outline_thickness = 0.0f;
	};

	struct texture_font_t
	{
		Vector* glyphs;

		texture_atlas_t * atlas;

		enum {
			TEXTURE_FONT_FILE = 0,
			TEXTURE_FONT_MEMORY,
		} location;

		union {
			char *filename;

			struct {
				const void *base;
				size_t size;
			} memory;
		};

		int hinting;
		int outline_type;
		int filtering;
		int kerning;

		unsigned char lcd_weights[5];

		float size;
		float outline_thickness;
		float height;
		float linegap;
		float ascender;
		float descender;

		float underline_position;
		float underline_thickness;
	};

	texture_font_t * texture_font_new_from_file(texture_atlas_t * atlas, const float pt_size, const char * filename);
	texture_font_t * texture_font_new_from_memory(texture_atlas_t *atlas, float pt_size, const void *memory_base, size_t memory_size);
	
	void texture_font_delete(texture_font_t * self);

	size_t texture_font_load_glyphs(texture_font_t * self, const wchar_t * charcodes);
	
	float texture_glyph_get_kerning(const texture_glyph_t * self, const wchar_t charcode);

	texture_glyph_t * texture_font_get_glyph(texture_font_t * self, wchar_t charcode);
	texture_glyph_t * texture_glyph_new(void);
}