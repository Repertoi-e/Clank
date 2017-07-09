#pragma once

#include "Event.h"

namespace cl { 
	
	class API EventDispatcher
	{
	private:
		Event& m_Event;           
	public:
		EventDispatcher(Event& event)        
			: m_Event(event) {}
		
		template <typename T>
		void Dispatch(std::function<bool(T&)> func)
		{
			if (cast(s32) T::GetStaticType() & cast(s32) m_Event.GetType())
			{
				BOOL handled = func(*cast(T*) &m_Event);
				m_Event.m_Handled = handled;
			}
		}
	};
}
