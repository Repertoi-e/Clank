#pragma once

#include <cstdint>

#include <type_traits>

#include <string>

typedef int8_t	 s8;
typedef int16_t	 s16;
typedef int32_t	 s32;
typedef int64_t	 s64;

typedef uint8_t	 u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef wchar_t  wchar; 

typedef u8		 byte;
typedef wchar    wbyte;

typedef float    float32;
typedef double   float64;

typedef std::wstring String;

namespace cl {

	template <typename T>
	struct Type { _STCONS(bool, Integer, false); 
				  _STCONS(bool, String, false);
				  _STCONS(u32, Size, 0);
				  _STCONS(u32, Signed, 0); };

	template <typename T, u32 S>
	struct IntType
	{
		_STCONS(bool, Integer, true); 
		_STCONS(bool, String, false);
		_STCONS(u32, Size, sizeof(T));
		_STCONS(u32, Signed, S);
	};

	template <> struct Type<s8>  : public IntType<s8, 1>  {  };
	template <> struct Type<s16> : public IntType<s16, 1> {  };
	template <> struct Type<s32> : public IntType<s32, 1> {  };
	template <> struct Type<s64> : public IntType<s64, 1> {  };
													
	template <> struct Type<u8>  : public IntType<u8, 0>  {  };
	template <> struct Type<u16> : public IntType<u16, 0> {  };
	template <> struct Type<u32> : public IntType<u32, 0> {  };
	template <> struct Type<u64> : public IntType<u64, 0> {  };

	template <typename T>
	struct StringType
	{
		_STCONS(bool, Integer, false);
		_STCONS(bool, String, true);
		_STCONS(u32, Size, sizeof(T));
		_STCONS(u32, Signed, 0);
	};

	template <> struct Type<char> : public StringType<char> { };
	template <> struct Type<wchar> : public StringType<wchar> { };
	template <> struct Type<String> : public StringType<String> { };

#define STRIP_CVREF(ty) std::remove_cv<std::remove_reference<ty>::type>::type
#define STRIP_CVPREF(ty) std::remove_cv<std::remove_pointer<std::remove_reference<ty>::type>::type>::type
}
