#include "cl/stdafx.h"
#include "WindowEvent.h"

namespace cl { 

	WindowResizeEvent::WindowResizeEvent(u32 width, u32 height)
		: Event(cast(EventType) GetStaticType()), m_Width(width), m_Height(height)
	{
	}

	WindowFocusEvent::WindowFocusEvent(bool focused)
		: Event(cast (EventType) GetStaticType()), m_Focused(focused)
	{
	}
}
