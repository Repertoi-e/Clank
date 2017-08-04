#pragma once

#include "cl/stdafx.h"

#include "cl/Maths/vec2.h"

namespace cl {

	struct API Rectangle 
	{
		union
		{
			vec2 position;
			struct
			{
				float32 x, y;
			};
		};
		union
		{
			vec2 size;
			struct
			{
				float32 width, height;
			};
		};

		Rectangle();
		Rectangle(const vec2& position, const vec2& size);
		Rectangle(float32 x, float32 y, float32 width, float32 height);

		inline const vec2 GetMin(void) const { return position - vec2(size.x, -size.y); }
		inline const vec2 GetMax(void) const { return position + vec2(size.x, -size.y); }

		inline const vec2 GetMinR(void) const { return position - size; }
		inline const vec2 GetMaxR(void) const { return position + size; }
	};
}