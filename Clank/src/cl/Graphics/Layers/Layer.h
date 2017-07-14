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
		Layer() {}
		~Layer() {}

		virtual void Init(Context* context) {}

		virtual void Tick() {}
		virtual void Event(Event& event) {}
		virtual void Render() {}
		virtual void Update(const DeltaTime& dt) {}

		bool& GetVisibility() { return m_Visible; }
	};
}