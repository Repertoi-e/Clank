#pragma once

#include "cl/stdafx.h"

#ifdef MOUSE_MOVED
#undef MOUSE_MOVED
#endif

namespace cl { 

	enum EventType : s32
	{
		MOUSE_CLICKED = BIT(0),
		MOUSE_RELEASED = BIT(1),
		MOUSE_MOVED = BIT(2),
		MOUSE_SCROLLED = BIT(3),

		KEY_PRESSED = BIT(4),
		KEY_RELEASED = BIT(5),

		WINDOW_RESIZE = BIT(6),
		WINDOW_FOCUS = BIT(7)
	};

	class API Event
	{
	public:
		friend class EventDispatcher;
	private:
		EventType m_Type;     
		bool m_Handled;     
	public:
		Event(const EventType& type)     
			: m_Type(type)
		{
		}

		const bool IsHandled(void) const { return m_Handled; }

		const EventType& GetType(void) const { return m_Type; }
	};
}