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

#include "api.h"

// Define common macros 
#define BIT(x) (1 << x)

#define NULLPTR 0
#define cast(x) (x)  
#define to_str(x) std::to_string(x)

#pragma warning(disable: 4251)

