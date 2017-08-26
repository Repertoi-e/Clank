#pragma once

#include "cl/stdafx.h"

namespace cl {

	class Renderer
	{
		virtual void Begin(void) = 0;
		virtual void End(void) = 0;
		virtual void Present(void) = 0;
	};
}