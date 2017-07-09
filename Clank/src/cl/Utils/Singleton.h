#pragma once

#include "INoCopy.h"

namespace cl {

	template <typename T>
	class Singleton : INoCopy
	{
	protected:
		explicit Singleton() {}
	public:
		static T& Instance()
		{
			static T instance;
			return instance;
		}
	};
}