#include "cl/stdafx.h"
#include "Rectangle.h"

namespace cl {

	Rectangle::Rectangle(void)
		: x(0), y(0), width(0), height(0)
	{
	}

	Rectangle::Rectangle(const vec2& position, const vec2& size)
		: position(position), size(size)
	{
	}
	Rectangle::Rectangle(float32 x, float32 y, float32 width, float32 height)
		: x(x), y(y), width(width), height(height)
	{
	}
}
