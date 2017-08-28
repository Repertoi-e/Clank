#include "cl/stdafx.h"
#include "FontTexture.h"

#include <assert.h>

#include "cl/Utils/Log.h"

namespace cl {
	
	static int
		texture_font_load_face(texture_font_t *self, float size,
			FT_Library *library, FT_Face *face)
	{
		FT_Error error;
		FT_Matrix matrix = {
			(int)((1.0 / HRES) * 0x10000L),
			(int)((0.0) * 0x10000L),
			(int)((0.0) * 0x10000L),
			(int)((1.0) * 0x10000L) };

		assert(library);
		assert(size);

		/* Initialize library */
		error = FT_Init_FreeType(library);
		if (error) {
			fprintf(stderr, "FT_Error (0x%02x) : %s\n",
				FT_Errors[error].code, FT_Errors[error].message);
			return 0;
		}

		/* Load face */
		switch (self->location) {
		case texture_font_t::TEXTURE_FONT_FILE:
			error = FT_New_Face(*library, self->filename, 0, face);
			break;

		case texture_font_t::TEXTURE_FONT_MEMORY:
			error = FT_New_Memory_Face(*library,
				cast(FT_Byte*) self->memory.base, self->memory.size, 0, face);
			break;
		}

		if (error) {
			fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
				__LINE__, FT_Errors[error].code, FT_Errors[error].message);
			FT_Done_FreeType(*library);
			return 0;
		}

		/* Select charmap */
		error = FT_Select_Charmap(*face, FT_ENCODING_UNICODE);
		if (error) {
			fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
				__LINE__, FT_Errors[error].code, FT_Errors[error].message);
			FT_Done_Face(*face);
			FT_Done_FreeType(*library);
			return 0;
		}

		/* Set char size */
		error = FT_Set_Char_Size(*face, (int)(size * HRES), 0, DPI * HRES, DPI);

		if (error) {
			fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
				__LINE__, FT_Errors[error].code, FT_Errors[error].message);
			FT_Done_Face(*face);
			FT_Done_FreeType(*library);
			return 0;
		}

		/* Set transform matrix */
		FT_Set_Transform(*face, &matrix, NULL);

		return 1;
	}

	static int
		texture_font_get_face_with_size(texture_font_t *self, float size,
			FT_Library *library, FT_Face *face)
	{
		return texture_font_load_face(self, size, library, face);
	}

	static int
		texture_font_get_face(texture_font_t *self,
			FT_Library *library, FT_Face *face)
	{
		return texture_font_get_face_with_size(self, self->size, library, face);
	}

	static int
		texture_font_get_hires_face(texture_font_t *self,
			FT_Library *library, FT_Face *face)
	{
		return texture_font_get_face_with_size(self,
			self->size * 100.f, library, face);
	}

	// ------------------------------------------------------ texture_glyph_new ---
	texture_glyph_t *
		texture_glyph_new(void)
	{
		texture_glyph_t *self = (texture_glyph_t *)malloc(sizeof(texture_glyph_t));
		if (self == NULL) {
			fprintf(stderr,
				"line %d: No more memory for allocating data\n", __LINE__);
			return NULL;
		}

		self->id = 0;
		self->width = 0;
		self->height = 0;
		self->outline_type = 0;
		self->outline_thickness = 0.0;
		self->offset_x = 0;
		self->offset_y = 0;
		self->advance_x = 0.0;
		self->advance_y = 0.0;
		self->s0 = 0.0;
		self->t0 = 0.0;
		self->s1 = 0.0;
		self->t1 = 0.0;
		self->kerning = anew Vector(sizeof(kerning_t));
		return self;
	}


	// --------------------------------------------------- texture_glyph_delete ---
	void
		texture_glyph_delete(texture_glyph_t *self)
	{
		assert(self);
		// ?
		del self->kerning;
		free(self);
	}

	float
		texture_glyph_get_kerning(const texture_glyph_t * self,
			const wchar_t charcode)
	{
		size_t i;

		assert(self);
		for (i = 0; i < self->kerning->Size(); ++i)
		{
			kerning_t * kerning = cast(kerning_t*) self->kerning->Get(i);
			if (kerning->charcode == charcode)
			{
				return kerning->kerning;
			}
		}
		return 0;
	}


	// ------------------------------------------ texture_font_generate_kerning ---
	void
		texture_font_generate_kerning(texture_font_t *self)
	{
		size_t i, j;
		FT_Library library;
		FT_Face face;
		FT_UInt glyph_index, prev_index;
		texture_glyph_t *glyph, *prev_glyph;
		FT_Vector kerning;

		assert(self);

		/* Load font */
		if (!texture_font_get_face(self, &library, &face))
			return;

		/* For each glyph couple combination, check if kerning is necessary */
		/* Starts at index 1 since 0 is for the special backgroudn glyph */
		for (i = 1; i<self->glyphs->Size(); ++i)
		{
			glyph = *cast(texture_glyph_t**) self->glyphs->Get(i);
			glyph_index = FT_Get_Char_Index(face, glyph->charcode);
			glyph->kerning->Clear();

			for (j = 1; j<self->glyphs->Size(); ++j)
			{
				prev_glyph = *cast(texture_glyph_t**)self->glyphs->Get(j);
				prev_index = FT_Get_Char_Index(face, prev_glyph->charcode);
				FT_Get_Kerning(face, prev_index, glyph_index, FT_KERNING_UNFITTED, &kerning);
				// printf("%c(%d)-%c(%d): %ld\n",
				//       prev_glyph->charcode, prev_glyph->charcode,
				//       glyph_index, glyph_index, kerning.x);
				if (kerning.x)
				{
					kerning_t k = { prev_glyph->charcode, kerning.x / (float)(HRESf*HRESf) };
					glyph->kerning->PushBack(&k);
				}
			}
		}

		FT_Done_Face(face);
		FT_Done_FreeType(library);
	}

	// ------------------------------------------------------ texture_font_init ---
	static int
		texture_font_init(texture_font_t *self)
	{
		FT_Library library;
		FT_Face face;
		FT_Size_Metrics metrics;

		assert(self->atlas);
		assert(self->size > 0);
		assert((self->location == texture_font_t::TEXTURE_FONT_FILE && self->filename)
			|| (self->location == texture_font_t::TEXTURE_FONT_MEMORY
				&& self->memory.base && self->memory.size));

		self->glyphs = anew Vector(sizeof(texture_glyph_t*));
		self->height = 0;
		self->ascender = 0;
		self->descender = 0;
		self->outline_type = 0;
		self->outline_thickness = 0.0;
		self->hinting = 1;
		self->kerning = 1;
		self->filtering = 1;

		// FT_LCD_FILTER_LIGHT   is (0x00, 0x55, 0x56, 0x55, 0x00)
		// FT_LCD_FILTER_DEFAULT is (0x10, 0x40, 0x70, 0x40, 0x10)
		self->lcd_weights[0] = 0x10;
		self->lcd_weights[1] = 0x40;
		self->lcd_weights[2] = 0x70;
		self->lcd_weights[3] = 0x40;
		self->lcd_weights[4] = 0x10;

		/* Get font metrics at high resolution */
		if (!texture_font_get_hires_face(self, &library, &face))
			return -1;

		self->underline_position = face->underline_position / (float)(HRESf*HRESf) * self->size;
		self->underline_position = round(self->underline_position);
		if (self->underline_position > -2)
		{
			self->underline_position = -2.0;
		}

		self->underline_thickness = face->underline_thickness / (float)(HRESf*HRESf) * self->size;
		self->underline_thickness = round(self->underline_thickness);
		if (self->underline_thickness < 1)
		{
			self->underline_thickness = 1.0;
		}

		metrics = face->size->metrics;
		self->ascender = (metrics.ascender >> 6) / 100.0;
		self->descender = (metrics.descender >> 6) / 100.0;
		self->height = (metrics.height >> 6) / 100.0;
		self->linegap = self->height - self->ascender + self->descender;
		FT_Done_Face(face);
		FT_Done_FreeType(library);

		/* -1 is a special glyph */
		texture_font_get_glyph(self, -1);

		return 0;
	}

	// --------------------------------------------- texture_font_new_from_file ---
	texture_font_t *
		texture_font_new_from_file(texture_atlas_t *atlas, const float pt_size,
			const char *filename)
	{
		texture_font_t *self;

		assert(filename);

		self = cast(texture_font_t*) calloc(1, sizeof(*self));
		if (!self) {
			fprintf(stderr,
				"line %d: No more memory for allocating data\n", __LINE__);
			return NULL;
		}

		self->atlas = atlas;
		self->size = pt_size;

		self->location = texture_font_t::TEXTURE_FONT_FILE;
		self->filename = strdup(filename);

		if (texture_font_init(self)) {
			texture_font_delete(self);
			return NULL;
		}

		return self;
	}

	// ------------------------------------------- texture_font_new_from_memory ---
	texture_font_t *
		texture_font_new_from_memory(texture_atlas_t *atlas, float pt_size,
			const void *memory_base, size_t memory_size)
	{
		texture_font_t *self;

		assert(memory_base);
		assert(memory_size);

		self = cast(texture_font_t*) calloc(1, sizeof(*self));
		if (!self) {
			fprintf(stderr,
				"line %d: No more memory for allocating data\n", __LINE__);
			return NULL;
		}

		self->atlas = atlas;
		self->size = pt_size;

		self->location = texture_font_t::TEXTURE_FONT_MEMORY;
		self->memory.base = memory_base;
		self->memory.size = memory_size;

		if (texture_font_init(self)) {
			texture_font_delete(self);
			return NULL;
		}

		return self;
	}

	// ---------------------------------------------------- texture_font_delete ---
	void
		texture_font_delete(texture_font_t *self)
	{
		size_t i;
		texture_glyph_t *glyph;

		assert(self);

		if (self->location == texture_font_t::TEXTURE_FONT_FILE && self->filename)
			free(self->filename);

		for (i = 0; i<self->glyphs->Size(); ++i)
		{
			glyph = *cast(texture_glyph_t**)self->glyphs->Get(i);
			texture_glyph_delete(glyph);
		}

		del self->glyphs;
		free(self);
	}


	// ----------------------------------------------- texture_font_load_glyphs ---
	size_t
		texture_font_load_glyphs(texture_font_t * self,
			const wchar_t * charcodes)
	{
		size_t i, j, x, y, width, height, depth, w, h;
		FT_Library library;
		FT_Error error;
		FT_Face face;
		FT_Glyph ft_glyph = NULL;
		FT_GlyphSlot slot;
		FT_Bitmap ft_bitmap;

		FT_UInt glyph_index;
		texture_glyph_t *glyph;
		FT_Int32 flags = 0;
		int ft_glyph_top = 0;
		int ft_glyph_left = 0;

		vec4i region;
		size_t missed = 0;
		char pass;

		assert(self);
		assert(charcodes);


		width = self->atlas->width;
		height = self->atlas->height;
		depth = self->atlas->depth;

		if (depth == 2)
			depth = 1;

		if (!texture_font_get_face(self, &library, &face))
			return wcslen(charcodes);

		/* Load each glyph */
		for (i = 0; i<wcslen(charcodes); ++i) {
			pass = 0;
			/* Check if charcode has been already loaded */
			for (j = 0; j < self->glyphs->Size(); ++j) {
				glyph = *cast(texture_glyph_t**)self->glyphs->Get(j);
				// If charcode is -1, we don't care about outline type or thickness
				// if( (glyph->charcode == charcodes[i])) {
				if ((glyph->charcode == charcodes[i]) &&
					((charcodes[i] == (wchar_t)(-1)) ||
					((glyph->outline_type == self->outline_type) &&
						(glyph->outline_thickness == self->outline_thickness))))
				{
					pass = 1;
					break;
				}
			}

			if (pass)
				continue; // don't add the item

			flags = 0;
			ft_glyph_top = 0;
			ft_glyph_left = 0;
			glyph_index = FT_Get_Char_Index(face, charcodes[i]);
			// WARNING: We use texture-atlas depth to guess if user wants
			//          LCD subpixel rendering

			if (self->outline_type > 0)
			{
				flags |= FT_LOAD_NO_BITMAP;
			}
			else
			{
				flags |= FT_LOAD_RENDER;
			}

			if (!self->hinting)
			{
				flags |= FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT;
			}
			else
			{
				flags |= FT_LOAD_FORCE_AUTOHINT;
			}


			if (depth == 3)
			{
				FT_Library_SetLcdFilter(library, FT_LCD_FILTER_LIGHT);
				flags |= FT_LOAD_TARGET_LCD;
				if (self->filtering)
				{
					FT_Library_SetLcdFilterWeights(library, self->lcd_weights);
				}
			}
			error = FT_Load_Glyph(face, glyph_index, flags);
			if (error)
			{
				fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
					__LINE__, FT_Errors[error].code, FT_Errors[error].message);
				FT_Done_Face(face);
				FT_Done_FreeType(library);
				return wcslen(charcodes) - i;
			}


			if (self->outline_type == 0)
			{
				slot = face->glyph;
				ft_bitmap = slot->bitmap;
				ft_glyph_top = slot->bitmap_top;
				ft_glyph_left = slot->bitmap_left;
			}
			else
			{
				FT_Stroker stroker;
				FT_BitmapGlyph ft_bitmap_glyph;
				error = FT_Stroker_New(library, &stroker);
				if (error)
				{
					fprintf(stderr, "FT_Error (0x%02x) : %s\n",
						FT_Errors[error].code, FT_Errors[error].message);
					FT_Done_Face(face);
					FT_Stroker_Done(stroker);
					FT_Done_FreeType(library);
					return 0;
				}
				FT_Stroker_Set(stroker,
					(int)(self->outline_thickness * HRES),
					FT_STROKER_LINECAP_ROUND,
					FT_STROKER_LINEJOIN_ROUND,
					0);
				error = FT_Get_Glyph(face->glyph, &ft_glyph);
				if (error)
				{
					fprintf(stderr, "FT_Error (0x%02x) : %s\n",
						FT_Errors[error].code, FT_Errors[error].message);
					FT_Done_Face(face);
					FT_Stroker_Done(stroker);
					FT_Done_FreeType(library);
					return 0;
				}

				if (self->outline_type == 1)
				{
					error = FT_Glyph_Stroke(&ft_glyph, stroker, 1);
				}
				else if (self->outline_type == 2)
				{
					error = FT_Glyph_StrokeBorder(&ft_glyph, stroker, 0, 1);
				}
				else if (self->outline_type == 3)
				{
					error = FT_Glyph_StrokeBorder(&ft_glyph, stroker, 1, 1);
				}
				if (error)
				{
					fprintf(stderr, "FT_Error (0x%02x) : %s\n",
						FT_Errors[error].code, FT_Errors[error].message);
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
						fprintf(stderr, "FT_Error (0x%02x) : %s\n",
							FT_Errors[error].code, FT_Errors[error].message);
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
						fprintf(stderr, "FT_Error (0x%02x) : %s\n",
							FT_Errors[error].code, FT_Errors[error].message);
						FT_Done_Face(face);
						FT_Stroker_Done(stroker);
						FT_Done_FreeType(library);
						return 0;
					}
				}
				ft_bitmap_glyph = (FT_BitmapGlyph)ft_glyph;
				ft_bitmap = ft_bitmap_glyph->bitmap;
				ft_glyph_top = ft_bitmap_glyph->top;
				ft_glyph_left = ft_bitmap_glyph->left;
				FT_Stroker_Done(stroker);
			}


			// We want each glyph to be separated by at least one black pixel
			// (for example for shader used in demo-subpixel.c)
			w = ft_bitmap.width / depth + 1;
			h = ft_bitmap.rows + 1;
			region = texture_atlas_get_region(self->atlas, w, h);
			if (region.x < 0)
			{
				missed++;
				fprintf(stderr, "Texture atlas is full (line %d)\n", __LINE__);
				continue;
			}
			w = w - 1;
			h = h - 1;
			x = region.x;
			y = region.y;
			texture_atlas_set_region(self->atlas, x, y, w, h,
				ft_bitmap.buffer, ft_bitmap.pitch);

			glyph = texture_glyph_new();
			glyph->charcode = charcodes[i];
			glyph->width = w;
			glyph->height = h;
			glyph->outline_type = self->outline_type;
			glyph->outline_thickness = self->outline_thickness;
			glyph->offset_x = ft_glyph_left;
			glyph->offset_y = ft_glyph_top;
			glyph->s0 = x / (float)width;
			glyph->t0 = y / (float)height;
			glyph->s1 = (x + glyph->width) / (float)width;
			glyph->t1 = (y + glyph->height) / (float)height;

			// Discard hinting to get advance
			FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
			slot = face->glyph;
			glyph->advance_x = slot->advance.x / HRESf;
			glyph->advance_y = slot->advance.y / HRESf;

			self->glyphs->PushBack(&glyph);

			if (self->outline_type > 0)
			{
				FT_Done_Glyph(ft_glyph);
			}
		}
		FT_Done_Face(face);
		FT_Done_FreeType(library);
		// SP: texture_atlas_upload( self->atlas );
		texture_atlas_upload(self->atlas);
		texture_font_generate_kerning(self);
		return missed;
	}


	// ------------------------------------------------- texture_font_get_glyph ---
	texture_glyph_t *
		texture_font_get_glyph(texture_font_t * self,
			wchar_t charcode)
	{
		size_t i;
		wchar_t buffer[2] = { 0,0 };
		texture_glyph_t *glyph;

		assert(self);

		assert(self);
		assert(self->filename);
		assert(self->atlas);

		/* Check if charcode has been already loaded */
		for (i = 0; i<self->glyphs->Size(); ++i)
		{
			glyph = *cast(texture_glyph_t**)self->glyphs->Get(i);
			// If charcode is -1, we don't care about outline type or thickness
			if ((glyph->charcode == charcode) &&
				((charcode == (wchar_t)(-1)) ||
				((glyph->outline_type == self->outline_type) &&
					(glyph->outline_thickness == self->outline_thickness))))
			{
				return glyph;
			}
		}

		/* charcode -1 is special : it is used for line drawing (overline,
		* underline, strikethrough) and background.
		*/
		if (charcode == (wchar_t)(-1))
		{
			size_t width = self->atlas->width;
			size_t height = self->atlas->height;
			vec4i region = texture_atlas_get_region(self->atlas, 5, 5);
			texture_glyph_t * glyph = texture_glyph_new();
			static unsigned char data[4 * 4 * 3] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
				-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
				-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
				-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
			if (region.x < 0)
			{
				fprintf(stderr, "Texture atlas is full (line %d)\n", __LINE__);
				return NULL;
			}
			texture_atlas_set_region(self->atlas, region.x, region.y, 4, 4, data, 0);
			glyph->charcode = (wchar_t)(-1);
			glyph->s0 = (region.x + 2) / (float)width;
			glyph->t0 = (region.y + 2) / (float)height;
			glyph->s1 = (region.x + 3) / (float)width;
			glyph->t1 = (region.y + 3) / (float)height;
			self->glyphs->PushBack(&glyph);
			return glyph; //*(texture_glyph_t **) vector_back( self->glyphs );
		}

		/* Glyph has not been already loaded */
		buffer[0] = charcode;
		if (texture_font_load_glyphs(self, buffer) == 0)
		{
			return *cast(texture_glyph_t**)self->glyphs->Back();
		}
		return NULL;
	}

	/*
	float32 texture_glyph_t::GetKerning(const wchar charcode)
	{
		for (u32 i = 0; i < m_Kerning.size(); ++i)
		{
			kerning_t* kerning = m_Kerning[i];
			if (kerning->charcode == charcode)
				return kerning->kerning;
		}
		return 0;
	}

	s32 FontTexture::LoadFace(float32 size, FT_Library* library, FT_Face* face)
	{
		FT_Error error;
		FT_Matrix matrix = {
			(s32)((1.0 / HRES) * 0x10000L),
			(s32)((0.0) * 0x10000L),
			(s32)((0.0) * 0x10000L),
			(s32)((1.0) * 0x10000L) };

		ASSERT(library, "Library is NULL");
		ASSERT(size, "Size is 0");

		error = FT_Init_FreeType(library);
		if (error) {
			LOG_ERROR("FT_Error (", FT_Errors[error].code, ") : ", FT_Errors[error].message, "\n");
			return 0;
		}

		switch (location) 
		{
		case TEXTURE_FONT_FILE:
			error = FT_New_Face(*library, filename, 0, face);
			break;

		case TEXTURE_FONT_MEMORY:
			error = FT_New_Memory_Face(*library, cast(FT_Byte*) memory.base, memory.size, 0, face);
			break;
		}

		if (error) 
		{
			LOG_ERROR("FT_Error (", FT_Errors[error].code, ") : ", FT_Errors[error].message, "\n");
			FT_Done_FreeType(*library);
			return 0;
		}

		error = FT_Select_Charmap(*face, FT_ENCODING_UNICODE);
		if (error) 
		{
			LOG_ERROR("FT_Error (", FT_Errors[error].code, ") : ", FT_Errors[error].message, "\n");
			FT_Done_Face(*face);
			FT_Done_FreeType(*library);
			return 0;
		}

		error = FT_Set_Char_Size(*face, (s32)(size * HRES), 0, DPI * HRES, DPI);

		if (error) 
		{
			LOG_ERROR("FT_Error (", FT_Errors[error].code, ") : ", FT_Errors[error].message, "\n");
			FT_Done_Face(*face);
			FT_Done_FreeType(*library);
			return 0;
		}

		FT_Set_Transform(*face, &matrix, NULL);

		return 1;
	}

	FontTexture::FontTexture(FontTextureAtlas* atlas, float32 ptSize, const void* data, u32 size)
		: atlas(atlas), size(ptSize), location(TEXTURE_FONT_MEMORY)
	{
		ASSERT(data, "Data is NULLPTR");
		ASSERT(size, "Size is 0");

		memory.base = data;
		memory.size = size;

		if (Init()) 
		{
			LOG_ERROR("Failed init!");
		}
	}

	FontTexture::~FontTexture()
	{
		if (location == TEXTURE_FONT_FILE && filename)
			free(filename);

		glyphs.clear();
	}

	s32 FontTexture::Init(void)
	{
		FT_Library library;
		FT_Face face;
		FT_Size_Metrics metrics;

		ASSERT(atlas, "");
		ASSERT(size > 0, "");
		ASSERT((location == TEXTURE_FONT_FILE && filename)
			|| (location == TEXTURE_FONT_MEMORY
				&& memory.base && memory.size), "");

		height = 0;
		ascender = 0;
		descender = 0;
		outline_type = 0;
		outline_thickness = 0.0;
		hinting = 1;
		kerning = 1;
		filtering = 1;

		// FT_LCD_FILTER_LIGHT   is (0x00, 0x55, 0x56, 0x55, 0x00)
		// FT_LCD_FILTER_DEFAULT is (0x10, 0x40, 0x70, 0x40, 0x10)
		lcd_weights[0] = 0x10;
		lcd_weights[1] = 0x40;
		lcd_weights[2] = 0x70;
		lcd_weights[3] = 0x40;
		lcd_weights[4] = 0x10;

		if (!GetHiresFace(&library, &face))
			return -1;

		underline_position = face->underline_position / (float)(HRESf*HRESf) * size;
		underline_position = round(underline_position);
		if (underline_position > -2)
		{
			underline_position = -2.0;
		}

		underline_thickness = face->underline_thickness / (float)(HRESf*HRESf) * size;
		underline_thickness = round(underline_thickness);
		if (underline_thickness < 1)
		{
			underline_thickness = 1.0;
		}

		metrics = face.size()->metrics;
		ascender = (metrics.ascender >> 6) / 100.0;
		descender = (metrics.descender >> 6) / 100.0;
		height = (metrics.height >> 6) / 100.0;
		linegap = height - ascender + descender;
		FT_Done_Face(face);
		FT_Done_FreeType(library);

		GetGlyph(-1);

		return 0;
	}

	texture_glyph_t* FontTexture::GetGlyph(wchar charcode)
	{
		ASSERT(filename, "");
		ASSERT(atlas, "");
		
		size_t i;
		wchar buffer[2] = { 0,0 };
		texture_glyph_t* glyph;

		for (i = 0; i < glyphs.size(); ++i)
		{
			glyph = *glyphs[i];
			// If charcode is -1, we don't care about outline type or thickness
			if ((glyph->charcode == charcode) &&
				((charcode == (wchar)(-1)) ||
				((glyph->outline_type == outline_type) &&
					(glyph->outline_thickness == outline_thickness))))
			{
				return glyph;
			}
		}

		if (charcode == (wchar_t)(-1))
		{
			size_t width = atlas->m_Width;
			size_t height = atlas->m_Height;
			vec4i region = atlas->GetRegion(5, 5);
			texture_glyph_t* aglyph = anew texture_glyph_t;
			static unsigned char data[4 * 4 * 3] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
				-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
				-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
				-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
			if (region.x < 0)
			{
				fprintf(stderr, "Texture atlas is full (line %d)\n", __LINE__);
				return NULL;
			}
			atlas->SetRegion(region.x, region.y, 4, 4, data, 0);
			aglyph->charcode = (wchar_t)(-1);
			aglyph->s0 = (region.x + 2) / (float)width;
			aglyph->t0 = (region.y + 2) / (float)height;
			aglyph->s1 = (region.x + 3) / (float)width;
			aglyph->t1 = (region.y + 3) / (float)height;
			glyphs.push_back(&aglyph);
			return aglyph; //*(texture_glyph_t **) vector_back( glyphs );
		}

		buffer[0] = charcode;
		if (LoadGlyphs(buffer) == 0)
		{
			return *(texture_glyph_t **)glyphs.back();
		}
		return NULL;
	}

	s32 FontTexture::GetFaceWithSize(float32 size, FT_Library* library, FT_Face* face)
	{
		return LoadFace(size, library, face);
	}

	s32 FontTexture::GetFace(FT_Library* library, FT_Face* face)
	{
		return GetFaceWithSize(size, library, face);
	}

	s32 FontTexture::GetHiresFace(FT_Library* library, FT_Face* face)
	{
		return GetFaceWithSize(size * 100.f, library, face);
	}

	u32 FontTexture::LoadGlyphs(const wchar * charcodes)
	{
		size_t i, j, x, y, width, height, depth, w, h;
		FT_Library library;
		FT_Error error;
		FT_Face face;
		FT_Glyph ft_glyph = NULL;
		FT_GlyphSlot slot;
		FT_Bitmap ft_bitmap;

		FT_UInt glyph_index;
		texture_glyph_t *glyph;
		FT_Int32 flags = 0;
		int ft_glyph_top = 0;
		int ft_glyph_left = 0;

		vec4i region;
		size_t missed = 0;
		char pass;

		ASSERT(charcodes, "");

		width = atlas->m_Width;
		height = atlas->m_Height;
		depth = atlas->m_Depth;

		if (depth == 2)
			depth = 1;

		if (!GetFace(&library, &face))
			return wcslen(charcodes);

		for (i = 0; i<wcslen(charcodes); ++i) {
			pass = 0;
			for (j = 0; j < glyphs.size(); ++j) {
				glyph = *(texture_glyph_t **)glyphs[j];
				// If charcode is -1, we don't care about outline type or thickness
				// if( (glyph->charcode == charcodes[i])) {
				if ((glyph->charcode == charcodes[i]) &&
					((charcodes[i] == (wchar_t)(-1)) ||
					((glyph->outline_type == outline_type) &&
						(glyph->outline_thickness == outline_thickness))))
				{
					pass = 1;
					break;
				}
			}

			if (pass)
				continue; // don't add the item

			flags = 0;
			ft_glyph_top = 0;
			ft_glyph_left = 0;
			glyph_index = FT_Get_Char_Index(face, charcodes[i]);
			// WARNING: We use texture-atlas depth to guess if user wants
			//          LCD subpixel rendering

			if (outline_type > 0)
			{
				flags |= FT_LOAD_NO_BITMAP;
			}
			else
			{
				flags |= FT_LOAD_RENDER;
			}

			if (!hinting)
			{
				flags |= FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT;
			}
			else
			{
				flags |= FT_LOAD_FORCE_AUTOHINT;
			}


			if (depth == 3)
			{
				FT_Library_SetLcdFilter(library, FT_LCD_FILTER_LIGHT);
				flags |= FT_LOAD_TARGET_LCD;
				if (filtering)
				{
					FT_Library_SetLcdFilterWeights(library, lcd_weights);
				}
			}
			error = FT_Load_Glyph(face, glyph_index, flags);
			if (error)
			{
				fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n",
					__LINE__, FT_Errors[error].code, FT_Errors[error].message);
				FT_Done_Face(face);
				FT_Done_FreeType(library);
				return wcslen(charcodes) - i;
			}


			if (outline_type == 0)
			{
				slot = face->glyph;
				ft_bitmap = slot->bitmap;
				ft_glyph_top = slot->bitmap_top;
				ft_glyph_left = slot->bitmap_left;
			}
			else
			{
				FT_Stroker stroker;
				FT_BitmapGlyph ft_bitmap_glyph;
				error = FT_Stroker_New(library, &stroker);
				if (error)
				{
					fprintf(stderr, "FT_Error (0x%02x) : %s\n",
						FT_Errors[error].code, FT_Errors[error].message);
					FT_Done_Face(face);
					FT_Stroker_Done(stroker);
					FT_Done_FreeType(library);
					return 0;
				}
				FT_Stroker_Set(stroker,
					(int)(outline_thickness * HRES),
					FT_STROKER_LINECAP_ROUND,
					FT_STROKER_LINEJOIN_ROUND,
					0);
				error = FT_Get_Glyph(face->glyph, &ft_glyph);
				if (error)
				{
					fprintf(stderr, "FT_Error (0x%02x) : %s\n",
						FT_Errors[error].code, FT_Errors[error].message);
					FT_Done_Face(face);
					FT_Stroker_Done(stroker);
					FT_Done_FreeType(library);
					return 0;
				}

				if (outline_type == 1)
				{
					error = FT_Glyph_Stroke(&ft_glyph, stroker, 1);
				}
				else if (outline_type == 2)
				{
					error = FT_Glyph_StrokeBorder(&ft_glyph, stroker, 0, 1);
				}
				else if (outline_type == 3)
				{
					error = FT_Glyph_StrokeBorder(&ft_glyph, stroker, 1, 1);
				}
				if (error)
				{
					fprintf(stderr, "FT_Error (0x%02x) : %s\n",
						FT_Errors[error].code, FT_Errors[error].message);
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
						fprintf(stderr, "FT_Error (0x%02x) : %s\n",
							FT_Errors[error].code, FT_Errors[error].message);
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
						fprintf(stderr, "FT_Error (0x%02x) : %s\n",
							FT_Errors[error].code, FT_Errors[error].message);
						FT_Done_Face(face);
						FT_Stroker_Done(stroker);
						FT_Done_FreeType(library);
						return 0;
					}
				}
				ft_bitmap_glyph = (FT_BitmapGlyph)ft_glyph;
				ft_bitmap = ft_bitmap_glyph->bitmap;
				ft_glyph_top = ft_bitmap_glyph->top;
				ft_glyph_left = ft_bitmap_glyph->left;
				FT_Stroker_Done(stroker);
			}


			// We want each glyph to be separated by at least one black pixel
			// (for example for shader used in demo-subpixel.c)
			w = ft_bitmap.width / depth + 1;
			h = ft_bitmap.rows + 1;
			region = atlas->GetRegion(w, h);
			if (region.x < 0)
			{
				missed++;
				fprintf(stderr, "Texture atlas is full (line %d)\n", __LINE__);
				continue;
			}
			w = w - 1;
			h = h - 1;
			x = region.x;
			y = region.y;
			atlas->SetRegion(x, y, w, h, ft_bitmap.buffer, ft_bitmap.pitch);

			glyph = anew texture_glyph_t();
			glyph->charcode = charcodes[i];
			glyph->width = w;
			glyph->height = h;
			glyph->outline_type = outline_type;
			glyph->outline_thickness = outline_thickness;
			glyph->offset_x = ft_glyph_left;
			glyph->offset_y = ft_glyph_top;
			glyph->s0 = x / (float32)width;
			glyph->t0 = y / (float32)height;
			glyph->s1 = (x + glyph->width) / (float32)width;
			glyph->t1 = (y + glyph->height) / (float32)height;

			// Discard hinting to get advance
			FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
			slot = face->glyph;
			glyph->advance_x = slot->advance.x / HRESf;
			glyph->advance_y = slot->advance.y / HRESf;

			glyphs.push_back(&glyph);

			if (outline_type > 0)
			{
				FT_Done_Glyph(ft_glyph);
			}
		}
		FT_Done_Face(face);
		FT_Done_FreeType(library);
		atlas->m_Dirty = 1;
		GenerateKerning();
		return missed;
	}

	void FontTexture::GenerateKerning(void)
	{
		size_t i, j;
		FT_Library library;
		FT_Face face;
		FT_UInt glyph_index, prev_index;
		texture_glyph_t *glyph, *prev_glyph;
		FT_Vector kerning;

		if (!GetFace(&library, &face))
			return;

		for (i = 1; i<glyphs.size(); ++i)
		{
			glyph = *(texture_glyph_t **)glyphs[i];
			glyph_index = FT_Get_Char_Index(face, glyph->charcode);
			glyph->m_Kerning.clear();

			for (j = 1; j<glyphs.size(); ++j)
			{
				prev_glyph = *(texture_glyph_t **)glyphs[j];
				prev_index = FT_Get_Char_Index(face, prev_glyph->charcode);
				FT_Get_Kerning(face, prev_index, glyph_index, FT_KERNING_UNFITTED, &kerning);
				// printf("%c(%d)-%c(%d): %ld\n",
				//       prev_glyph->charcode, prev_glyph->charcode,
				//       glyph_index, glyph_index, kerning.x);
				if (kerning.x)
				{
					kerning_t k = { prev_glyph->charcode, kerning.x / (float)(HRESf*HRESf) };
					glyph->m_Kerning.push_back(&k);
				}
			}
		}

		FT_Done_Face(face);
		FT_Done_FreeType(library);
	}
	*/
}