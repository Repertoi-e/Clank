#pragma once

#include "cl/stdafx.h"

#include "cl/Application/Events/Events.h"
#include "cl/Graphics/Context.h"
#include "cl/Maths/mat4.h"
#include "cl/Utils/Time.h"

namespace cl {

	class Layer
	{
	private:
		bool m_Visible;
	public:
		virtual ~Layer(void) = default;

		virtual void OnInit(void) = 0;
		virtual void OnEvent(Event& event) = 0;
		virtual void OnRender(void) = 0;
		virtual void OnUpdate(const DeltaTime& dt) = 0;
		virtual void OnTick(void) = 0;

		bool& GetVisibility(void) { return m_Visible; }
	};
}