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
		Layer(void) {}
		virtual ~Layer(void) = default;

		virtual void Init(Context* context) {}

		virtual void Tick(void) {}
		virtual void Event(Event& event) {}
		virtual void Render(void) {}
		virtual void Update(const DeltaTime& dt) {}

		bool& GetVisibility() { return m_Visible; }
	};
}