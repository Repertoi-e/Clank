// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <sstream>

#include <stack>
#include <array>
#include <list>
#include <map>
#include <unordered_map>
#include <memory>
#include <cstdint>

#include <cstddef>
#include <stdio.h>
#include <math.h>

#include "String.h"
#include "types.h"

// Define common macros 
#ifdef _LIB
	#ifdef _DLL
		#define API __declspec(dllexport)
	#else
		#define API __declspec(dllimport)
	#endif
#endif

#define BIT(x) (1 << x)

#define NULLPTR 0
#define cast(x) (x)  
#define str(x) std::to_string(x)
