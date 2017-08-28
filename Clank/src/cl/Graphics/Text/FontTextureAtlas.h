#pragma once

#include "cl/stdafx.h"

#include "cl/Maths/maths.h"
#include "cl/Utils/Vector.h"

namespace cl {

	struct texture_atlas_t
	{
		Vector* nodes;

		size_t width;
		size_t height;
		size_t depth;
		size_t used;

		unsigned int id;
		unsigned char * data;

		int dirty;
	};

	texture_atlas_t * texture_atlas_new(const size_t width, const size_t height, const size_t depth);

	void texture_atlas_delete(texture_atlas_t * self);
	void texture_atlas_upload(texture_atlas_t * self);
	void texture_atlas_set_region(texture_atlas_t * self, const size_t x, const size_t y, const size_t width, const size_t height, const unsigned char *data, const size_t stride);
	void texture_atlas_clear(texture_atlas_t * self);

	vec4i texture_atlas_get_region(texture_atlas_t * self, const size_t width, const size_t height);
}