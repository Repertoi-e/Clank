// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

#include "Memory/Memory.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <thread>

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

#include "api.h"
#include "dxerr.h"

// Define common macros 
#ifdef _DEBUG
	#define HR(x) \
	{ \
		HRESULT hr = x; \
		if(FAILED(hr)) \
		{ \
			DXTraceW(__FILEW__, __LINE__, hr, L#x, TRUE); \
		} \
	}
#else  
	#define HR(x) x;
#endif

#define BIT(x) (1 << x)

#define NULLPTR 0
#define cast(x) (x)  

#pragma warning(disable: 4251)

#pragma comment(lib, "dxgi.lib")