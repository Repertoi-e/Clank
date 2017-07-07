#pragma once

#include "Event.h"

#include "Keys.h"

namespace cl { 

	class API KeyEvent : public Event
	{
	private:
		s32 m_Button;         
		s32 m_Mods;        
	public:
		KeyEvent(EventType type, s32 button, s32 mods);        

		const s32 GetButton() const { return m_Button; }
		const bool IsModifier(s32 mod) const { return (m_Mods & mod); };
	};

	class API KeyPressedEvent : public KeyEvent
	{
	private:
		s32 m_Repeat;          
	public:
		KeyPressedEvent(s32 button, s32 mods, bool repeat);         

		const bool Repeat() const { return m_Repeat; }
		static s32 GetStaticType() { return EventType::KEY_PRESSED; }
	};
	
	class API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(s32 button, s32 mods);        

		static s32 GetStaticType() { return EventType::KEY_RELEASED; }
	};
}
