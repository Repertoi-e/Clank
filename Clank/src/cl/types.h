#pragma once

#include <cstdint>
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

	template<class T>
	class Type
	{
	public:
		static constexpr bool Integer = false;
		static constexpr bool Signed = false;
	};

	#define INTEGER_TYPE(type, sign) template<> class Type<type> \
	{ \
	public: \
		static constexpr bool Integer = true; \
		static constexpr bool Signed = sign; \
	}; \
	template<> class Type<const type> \
	{ \
	public: \
		static constexpr bool Integer = true; \
		static constexpr bool Signed = sign; \
	}; \
	template<> class Type<type&> \
	{ \
	public: \
		static constexpr bool Integer = true; \
		static constexpr bool Signed = sign; \
	}; \
	template<> class Type<const type&> \
	{ \
	public: \
		static constexpr bool Integer = true; \
		static constexpr bool Signed = sign; \
	};

	INTEGER_TYPE(s8, true);
	INTEGER_TYPE(s16, true);
	INTEGER_TYPE(s32, true);
	INTEGER_TYPE(s64, true);

	INTEGER_TYPE(u8, false);
	INTEGER_TYPE(u16, false);
	INTEGER_TYPE(u32, false);
	INTEGER_TYPE(u64, false);

	constexpr u32 Base(u32 base)
	{
		return base;
	}
}
