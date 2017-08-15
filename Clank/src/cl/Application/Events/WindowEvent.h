#pragma once

#include "Event.h"

namespace cl { 

	class API WindowResizeEvent : public Event
	{
	private:
		u32 m_Width;     
		u32 m_Height;     
	public:
		WindowResizeEvent(u32 width, u32 height);        

		const u32 GetWidth(void) const { return m_Width; }
		const u32 GetHeight(void) const { return m_Height; }

		static s32 GetStaticType(void) { return EventType::WINDOW_RESIZE; }
	};

	class API WindowFocusEvent : public Event
	{
	private:
		s32 m_Focused;        
	public:
		WindowFocusEvent(bool focused);

		const s32 Focused(void) const { return m_Focused; }

		static s32 GetStaticType(void) { return EventType::WINDOW_FOCUS; }
	};
}
