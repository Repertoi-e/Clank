#pragma once

#include "cl/stdafx.h"

#include "cl/Maths/maths.h"

#include "cl/Memory/Vector.h"

namespace cl {

	class FontTextureAtlas
	{
	private:
		friend class Font;
		friend class FontTexture;
		
		Vector* m_Nodes;

		u32 m_Width;
		u32 m_Height;
		u32 m_Depth;
		u32 m_Used = 0;

		byte* m_Data = NULLPTR;

		s32 m_Dirty = 0;
	public:
		FontTextureAtlas(u32 width, u32 height, u32 depth);
		~FontTextureAtlas();
		
		void Clear(void);

		void SetRegion(u32 x, u32 y, u32 width, u32 height, const byte* data, u32 stride);
		void SetDirty(s32 value);

		vec4i GetRegion(u32 width, u32 height);
	private:
		s32 Fit(u32 index, u32 width, u32 height);
		
		void Merge(void);
	};
}