#include "cl/stdafx.h"
#include "FontTextureAtlas.h"

#include "cl/Utils/Log.h"

#include <assert.h>

namespace cl {

	texture_atlas_t * texture_atlas_new(const size_t width, const size_t height, const size_t depth)
	{
		texture_atlas_t *self = anew texture_atlas_t;

		// We want a one pixel border around the whole atlas to avoid any artefact when
		// sampling texture
		vec3i node = vec3i(1, 1, width - 2);

		assert((depth == 1) || (depth == 2) || (depth == 3) || (depth == 4));
		if (self == NULL)
		{
			fprintf(stderr,
				"line %d: No more memory for allocating data\n", __LINE__);
			exit(EXIT_FAILURE);
		}
		self->used = 0;
		self->width = width;
		self->height = height;
		self->depth = depth;
		self->id = 0;
		self->dirty = 0;
		
		self->nodes = anew Vector(sizeof(vec3i));
		self->nodes->PushBack(&node);
		self->data = (unsigned char *)
			calloc(width*height*depth, sizeof(unsigned char));

		if (self->data == NULL)
		{
			fprintf(stderr,
				"line %d: No more memory for allocating data\n", __LINE__);
			exit(EXIT_FAILURE);
		}

		return self;
	}

	void texture_atlas_delete(texture_atlas_t *self)
	{
		assert(self);
		del self->nodes;
		if (self->data)
		{
			free(self->data);
		}
		if (self->id)
		{
			// SP: glDeleteTextures( 1, &self->id );
		}
		free(self);
	}

	void texture_atlas_set_region(texture_atlas_t * self, const size_t x, const size_t y, const size_t width, const size_t height, const unsigned char *data, const size_t stride)
	{
		size_t i, j;
		size_t depth;
		size_t charsize;
		unsigned char *row, *src;

		assert(self);
		assert(x > 0);
		assert(y > 0);
		assert(x < (self->width - 1));
		assert((x + width) <= (self->width - 1));
		assert(y < (self->height - 1));
		assert((y + height) <= (self->height - 1));

		charsize = sizeof(char);

		depth = self->depth;
		for (i = 0; i<height; ++i)
		{
			if (depth == 2)
			{
				row = self->data + ((y + i) * self->width + x) * charsize * depth;
				src = (unsigned char*) data + (i * stride) * charsize;
				for (j = 0; j < width; j++)
				{
					row[j * 2 + 0] = 0xff;
					row[j * 2 + 1] = src[j];
				}
			}
			else
			{
				memcpy(self->data + ((y + i)*self->width + x) * charsize * depth,
					data + (i*stride) * charsize, width * charsize * depth);
			}
		}
	}

	int texture_atlas_fit(texture_atlas_t * self, const size_t index, const size_t width, const size_t height)
	{
		vec3i *node;
		int x, y, width_left;
		size_t i;

		assert(self);

		node = cast(vec3i*) self->nodes->Get(index);
		x = node->x;
		y = node->y;
		width_left = width;
		i = index;

		if ((x + width) > (self->width - 1))
		{
			return -1;
		}
		y = node->y;
		while (width_left > 0)
		{
			node = cast(vec3i*) self->nodes->Get(i);
			if (node->y > y)
			{
				y = node->y;
			}
			if ((y + height) > (self->height - 1))
			{
				return -1;
			}
			width_left -= node->z;
			++i;
		}
		return y;
	}

	void texture_atlas_merge(texture_atlas_t * self)
	{
		vec3i *node, *next;
		size_t i;

		assert(self);

		for (i = 0; i< self->nodes->Size() - 1; ++i)
		{
			node = cast(vec3i*) self->nodes->Get(i);
			next = cast(vec3i*) self->nodes->Get(i + 1);
			if (node->y == next->y)
			{
				node->z += next->z;
				self->nodes->Erase(i + 1);
				--i;
			}
		}
	}

	vec4i texture_atlas_get_region(texture_atlas_t * self, const size_t width, const size_t height)
	{

		int y, best_height, best_width, best_index;
		vec3i *node, *prev;
		vec4i region = vec4i(0, 0, width, height);
		size_t i;

		assert(self);

		best_height = INT_MAX;
		best_index = -1;
		best_width = INT_MAX;
		for (i = 0; i< self->nodes->Size(); ++i)
		{
			y = texture_atlas_fit(self, i, width, height);
			if (y >= 0)
			{
				node = cast(vec3i*) self->nodes->Get(i);
				if (((y + height) < best_height) ||
					(((y + height) == best_height) && (node->z < best_width)))
				{
					best_height = y + height;
					best_index = i;
					best_width = node->z;
					region.x = node->x;
					region.y = y;
				}
			}
		}

		if (best_index == -1)
		{
			region.x = -1;
			region.y = -1;
			region.z = 0;
			region.w = 0;
			return region;
		}

		node = (vec3i *)malloc(sizeof(vec3i));
		if (node == NULL)
		{
			fprintf(stderr,
				"line %d: No more memory for allocating data\n", __LINE__);
			exit(EXIT_FAILURE);
		}
		node->x = region.x;
		node->y = region.y + height;
		node->z = width;
		self->nodes->Insert(best_index, node);
		free(node);

		for (i = best_index + 1; i < self->nodes->Size(); ++i)
		{
			node = cast(vec3i*) self->nodes->Get(i);
			prev = cast(vec3i*) self->nodes->Get(i - 1);

			if (node->x < (prev->x + prev->z))
			{
				int shrink = prev->x + prev->z - node->x;
				node->x += shrink;
				node->z -= shrink;
				if (node->z <= 0)
				{
					self->nodes->Erase(i);
					--i;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		texture_atlas_merge(self);
		self->used += width * height;
		return region;
	}

	void texture_atlas_clear(texture_atlas_t * self)
	{
		vec3i node = vec3i(1, 1, 1);

		assert(self);
		assert(self->data);

		self->nodes->Clear();
		self->used = 0;
		// We want a one pixel border around the whole atlas to avoid any artefact when
		// sampling texture
		node.z = self->width - 2;

		self->nodes->PushBack(&node);
		memset(self->data, 0, self->width*self->height*self->depth);
	}

	void texture_atlas_upload(texture_atlas_t * self)
	{
		assert(self);

		self->dirty = 1;
	}

	/*
	FontTextureAtlas::FontTextureAtlas(u32 width, u32 height, u32 depth)
		: m_Width(width), m_Height(height), m_Depth(depth)
	{
		// We want a one pixel border around the whole atlas to avoid any artefact when
		// sampling texture
		vec3i node = vec3i(1, 1, width - 2);
		m_Nodes.push_back(node);

		m_Data = cast(byte*) calloc(width * height * depth, sizeof(byte));
	}

	FontTextureAtlas::~FontTextureAtlas()
	{
		LOG_INFO("Calling destructor for FontTextureAltas");
		m_Nodes.clear();

		del m_Data;
	}

	void FontTextureAtlas::Clear(void)
	{
		// We want a one pixel border around the whole atlas to avoid any artefact when
		// sampling texture
		vec3i node = vec3i(1, 1, m_Width - 2);

		m_Used = 0;

		m_Nodes.clear();
		m_Nodes.push_back(node);
		
		if (m_Data)
			memset(m_Data, 0, m_Width * m_Height * m_Depth);
	}

	void FontTextureAtlas::SetRegion(u32 x, u32 y, u32 width, u32 height, byte* data, u32 stride)
	{
		ASSERT(x > 0, "Out of bounds");
		ASSERT(y > 0, "Out of bounds");
		ASSERT(x < (m_Width - 1), "Out of bounds");
		ASSERT((x + width) <= (m_Width - 1), "Out of bounds");
		ASSERT(y < (m_Height - 1), "Out of bounds");
		ASSERT((y + height) <= (m_Height - 1), "Out of bounds");

		for (u32 i = 0; i < height; ++i)
		{
			if (m_Depth == 2)
			{
				byte* row = m_Data + ((y + i) * m_Width + x) * m_Depth;
				byte* src = data + (i * stride);
				for (u32 j = 0; j < width; j++)
				{
					row[j * 2 + 0] = 0xff;
					row[j * 2 + 1] = src[j];
				}
			}
			else
			{
				memcpy(m_Data + ((y + i) * m_Width + x) * m_Depth, data + (i * stride), width * m_Depth);
			}
		}
	}

	void FontTextureAtlas::Merge(void)
	{
		for (u32 i = 0; i < m_nodes->Size() - 1; ++i)
		{
			vec3i node = m_Nodes[i];
			vec3i next = m_Nodes[i + 1];
			if (node.y == next.y)
			{
				node.z += next.z;
				m_Nodes.erase(m_Nodes.begin() + i + 1);
				--i;
			}
		}
	}

	vec4i FontTextureAtlas::GetRegion(u32 width, u32 height)
	{
		int y, best_height, best_width, best_index;
		vec4i region = vec4i(0, 0, width, height);
		size_t i;

		best_height = INT_MAX;
		best_index = -1;
		best_width = INT_MAX;
		for (i = 0; i < m_nodes->Size(); ++i)
		{
			y = Fit(i, width, height);
			if (y >= 0)
			{
				vec3i& node = m_Nodes[i];
				if (((y + height) < best_height) ||
					(((y + height) == best_height) && (node.z < best_width)))
				{
					best_height = y + height;
					best_index = i;
					best_width = node.z;
					region.x = node.x;
					region.y = y;
				}
			}
		}

		if (best_index == -1)
		{
			region.x = -1;
			region.y = -1;
			region.z = 0;
			region.w = 0;
			return region;
		}

		vec3i node;
		node.x = region.x;
		node.y = region.y + height;
		node.z = width;
		m_Nodes.insert(m_Nodes.begin() + best_index, node);

		for (i = best_index + 1; i < m_nodes->Size(); ++i)
		{
			vec3i& node = m_Nodes[i];
			vec3i& prev = m_Nodes[i - 1];

			if (node.x < (prev.x + prev.z))
			{
				int shrink = prev.x + prev.z - node.x;
				node.x += shrink;
				node.z -= shrink;
				if (node.z <= 0)
				{
					m_Nodes.erase(m_Nodes.begin() + i);
					--i;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		Merge();
		m_Used += width * height;
		return region;
	}

	s32 FontTextureAtlas::Fit(u32 index, u32 width, u32 height)
	{
		vec3i& node = m_Nodes[index];
		int x, y, width_left;
		size_t i;

		x = node.x;
		y = node.y;
		width_left = width;
		i = index;

		if ((x + width) > (m_Width - 1))
		{
			return -1;
		}
		y = node.y;
		while (width_left > 0)
		{
			node = m_Nodes[i];
			if (node.y > y)
			{
				y = node.y;
			}
			if ((y + height) > (m_Height - 1))
			{
				return -1;
			}
			width_left -= node.z;
			++i;
		}
		return y;
	}
	*/
}