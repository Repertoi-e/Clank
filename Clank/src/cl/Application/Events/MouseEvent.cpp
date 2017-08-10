#include "cl/stdafx.h"
#include "MouseEvent.h"

namespace cl { 

	MouseMovedEvent::MouseMovedEvent(s32 x, s32 y, bool dragged)
		: Event(cast(EventType) GetStaticType()), m_Dragged(dragged), m_x(x), m_y(y)
	{
	}

	MouseButtonEvent::MouseButtonEvent(EventType type, s32 x, s32 y)
		: Event(type), m_x(x), m_y(y)
	{
	}

	MouseClickedEvent::MouseClickedEvent(s32 x, s32 y, s32 button, s32 mods)
		: MouseButtonEvent(cast(EventType) GetStaticType(), x, y), m_Button(button), m_Mods(mods)
	{
	}

	MouseReleasedEvent::MouseReleasedEvent(s32 x, s32 y, s32 button, s32 mods)
		: MouseButtonEvent(cast(EventType) GetStaticType(), x, y), m_Button(button), m_Mods(mods)
	{
	}

	MouseScrollEvent::MouseScrollEvent(s32 x, s32 y, short distance)
		: Event(cast(EventType) GetStaticType()), m_Distance(distance), m_x(x), m_y(y)
	{
	}
}
