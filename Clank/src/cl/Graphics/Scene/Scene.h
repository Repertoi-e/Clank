#pragma once

#include "cl/stdafx.h"

#include "cl/Application/Events/Events.h"
#include "cl/Utils/Time.h"

namespace cl {

	class Scene
	{
	private:
		bool m_Active;
	public:
		virtual void OnPush(void) = 0;
		virtual void OnPop(void) = 0;

		virtual void OnEvent(Event& event) = 0;
		virtual void OnRender(void) = 0;
		virtual void OnUpdate(const DeltaTime& dt) = 0;
		virtual void OnTick(void) = 0;

		inline void SetActive(bool active) { m_Active = active; }
		inline const bool IsActive() const { return m_Active; }
	};
}