#include "cl/stdafx.h"
#include "KeyEvent.h"

namespace cl { 

	KeyEvent::KeyEvent(EventType type, s32 button, s32 mods)
		: Event(type), m_Button(button), m_Mods(mods)
	{
	}

	KeyPressedEvent::KeyPressedEvent(s32 button, s32 mods, bool repeat)
		: KeyEvent(cast(EventType) GetStaticType(), button, mods), m_Repeat(repeat)
	{
	}

	KeyReleasedEvent::KeyReleasedEvent(s32 button, s32 mods)
		: KeyEvent(cast(EventType) GetStaticType(), button, mods)
	{
	}
}
