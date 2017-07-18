#pragma once

#include "INoCopy.h"

namespace cl {

	template <typename T>
	class Singleton : INoCopy
	{
	protected:
		explicit Singleton(void) {}
	public:
		static T& Instance(void)
		{
			static T instance;
			return instance;
		}
	};
}