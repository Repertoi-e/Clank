#pragma once

#ifdef _LIB
	#ifdef _DLL
		#define API __declspec(dllexport)
	#else
		#define API __declspec(dllimport)
	#endif
#endif