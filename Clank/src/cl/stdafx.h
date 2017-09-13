// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

#include <any>
#include <algorithm>
#include <functional>

#include <string>
#include <iostream>
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

#include "Memory/Memory.h"

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

#define SafeRelease(x)	\
	if (x)				\
	{					\
		x->Release();	\
		x = NULLPTR;	\
	}

// Disable annoying useless warnings...
#pragma warning(disable: 4595)
#pragma warning(disable: 4251)
#pragma warning(disable: 4984)

#pragma comment(lib, "dxgi.lib")