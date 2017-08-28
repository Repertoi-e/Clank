#include "cl/stdafx.h"
#include "FontTextureAtlas.h"

#include "cl/Utils/Log.h"

#include <assert.h>

namespace cl {

	FontTextureAtlas::FontTextureAtlas(u32 width, u32 height, u32 depth)
		: m_Width(width), m_Height(height), m_Depth(depth)
	{
		vec3i node = vec3i(1, 1, width - 2);

		ASSERT((depth == 1) || (depth == 2) || (depth == 3) || (depth == 4), "Invalid depth");
		
		m_Nodes = anew Vector(sizeof(vec3i));
		m_Nodes->PushBack(&node);

		m_Data = anew byte[m_Width * m_Height * m_Depth];
	}

	FontTextureAtlas::~FontTextureAtlas()
	{
		del m_Nodes;
		del m_Data;
	}

	void FontTextureAtlas::Clear(void)
	{
		vec3i node = vec3i(1, 1, m_Width - 2);
		m_Nodes->Clear();
		m_Nodes->PushBack(&node);

		m_Used = 0;

		memset(m_Data, 0, m_Width * m_Height * m_Depth);
	}

	void FontTextureAtlas::SetRegion(u32 x, u32 y, u32 width, u32 height, const byte* data, u32 stride)
	{
		byte *row, *src;

		ASSERT(x < (m_Width - 1), "Index out of bounds");
		ASSERT((x + width) <= (m_Width - 1), "Index out of bounds");
		ASSERT(y < (m_Height - 1), "Index out of bounds");
		ASSERT((y + height) <= (m_Height - 1), "Index out of bounds");

		for (u32 i = 0; i<height; ++i)
		{
			if (m_Depth == 2)
			{
				row = m_Data + ((y + i) * m_Width + x) * m_Depth;
				src = (byte*) data + (i * stride);
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

	void FontTextureAtlas::SetDirty(s32 value)
	{
		m_Dirty = value;
	}

	void FontTextureAtlas::Merge(void)
	{
		vec3i* node;
		vec3i* next;

		for (u32 i = 0; i< m_Nodes->Size() - 1; ++i)
		{
			node = cast(vec3i*) m_Nodes->Get(i);
			next = cast(vec3i*) m_Nodes->Get(i + 1);
			if (node->y == next->y)
			{
				node->z += next->z;
				m_Nodes->Erase(i + 1);
				--i;
			}
		}
	}

	s32 FontTextureAtlas::Fit(u32 index, u32 width, u32 height)
	{
		vec3i* node = cast(vec3i*) m_Nodes->Get(index);

		s32 x = node->x;
		s32 y = node->y;

		if ((x + width) > (m_Width - 1))
			return -1;

		u32 i = index;
		s32 width_left = width;
		while (width_left > 0)
		{
			node = cast(vec3i*) m_Nodes->Get(i);
			if (node->y > y)
				y = node->y;
			if ((y + height) > (m_Height - 1))
				return -1;
			width_left -= node->z;
			++i;
		}

		return y;
	}

	vec4i FontTextureAtlas::GetRegion(u32 width, u32 height)
	{
		vec3i* node;
		vec3i* prev;
		vec4i region = vec4i(0, 0, width, height);

		s32 best_height = INT_MAX;
		s32 best_index = -1;
		s32 best_width = INT_MAX;
		for (u32 i = 0; i< m_Nodes->Size(); ++i)
		{
			s32 y = Fit(i, width, height);
			if (y >= 0)
			{
				node = cast(vec3i*) m_Nodes->Get(i);
				if (((y + cast(s32) height) < best_height) ||
					(((y + cast(s32) height) == best_height) && (node->z < best_width)))
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

		node = anew vec3i(region.x, region.y + height, width);
		m_Nodes->Insert(best_index, node);
		del node;

		for (u32 i = best_index + 1; i < m_Nodes->Size(); ++i)
		{
			node = cast(vec3i*) m_Nodes->Get(i);
			prev = cast(vec3i*) m_Nodes->Get(i - 1);

			if (node->x < (prev->x + prev->z))
			{
				s32 shrink = prev->x + prev->z - node->x;
				node->x += shrink;
				node->z -= shrink;
				if (node->z <= 0)
				{
					m_Nodes->Erase(i);
					--i;
				}
				else
					break;
			}
			else
				break;
		}
		Merge();
		
		m_Used += width * height;
		
		return region;
	}
}