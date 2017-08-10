#pragma once

#include "Event.h"

#include "Keys.h"

namespace cl { 

	class API MouseMovedEvent : public Event
	{
	private:
		s32 m_x;       
		s32 m_y;       
		bool m_Dragged;           
	public:
		MouseMovedEvent(s32 x, s32 y, bool dragged);         

		const bool IsDragged() const { return m_Dragged; }
		const s32 GetX() const { return m_x; }
		const s32 GetY() const { return m_y; }

		static s32 GetStaticType() { return EventType::MOUSE_MOVED; }
	};

	class API MouseScrollEvent : public Event
	{
	private:
		s32 m_x;
		s32 m_y;
		short m_Distance;
	public:
		MouseScrollEvent(s32 x, s32 y, short distance);

		const s32 GetX() const { return m_x; }
		const s32 GetY() const { return m_y; }

		const short GetDistance() const { return m_Distance; }

		static s32 GetStaticType() { return EventType::MOUSE_SCROLLED; }
	};

	class API MouseButtonEvent : public Event
	{
	private:
		s32 m_x;          
		s32 m_y;          
	public:
		MouseButtonEvent(EventType type, s32 x, s32 y);           

		const s32 GetX() const { return m_x; }
		const s32 GetY() const { return m_y; }
	};

	class API MouseClickedEvent : public MouseButtonEvent
	{
	private:
		s32 m_Button;         
		s32 m_Mods;          
	public:
		MouseClickedEvent(s32 x, s32 y, s32 button, s32 mods);           

		const s32 GetButton() const { return m_Button; }
		const bool IsModifier(s32 mod) const { return (m_Mods & mod); };

		static s32 GetStaticType() { return EventType::MOUSE_CLICKED; }
	};

	class API MouseReleasedEvent : public MouseButtonEvent
	{
	private:
		s32 m_Button;         
		s32 m_Mods;          
	public:
		MouseReleasedEvent(s32 x, s32 y, s32 button, s32 mods);           

		const s32 GetButton() const { return m_Button; }
		const bool IsModifier(s32 mod) const { return (m_Mods & mod); };

		static s32 GetStaticType() { return EventType::MOUSE_RELEASED; }
	};
}
