#pragma once

#include "cl/stdafx.h"

namespace cl {

	class API INoCopy
	{
	protected:
		INoCopy(void) {}
		~INoCopy(void) {}
	private:
		INoCopy(const INoCopy& other);
		const INoCopy& operator=(const INoCopy other);
	};
}