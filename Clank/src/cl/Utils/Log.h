#pragma once

#include "cl/stdafx.h"

#define LOG_LEVEL_FATAL  0
#define LOG_LEVEL_WARN	 1
#define LOG_LEVEL_ERROR	 2
#define LOG_LEVEL_INFO	 3

namespace cl {

	static char sprintf_buffer[1024 * 10];

	template <typename T>
	static const char* to_string(const T& t)
	{
		return typeid(T).name();
	}

	template <>
	static const char* to_string<char>(const char& t)
	{
		return &t;
	}

	template <>
	static const char* to_string<char*>(char* const& t)
	{
		return t;
	}

	template <>
	static const char* to_string<char const*>(char const* const& t)
	{
		return t;
	}

	template <>
	static const char* to_string<unsigned char const*>(unsigned char const* const& t)
	{
		return cast(const char*) t;
	}

	template <>
	static const char* to_string<bool>(const bool& t)
	{
		return t ? "true" : "false";
	}

	template <>
	static const char* to_string<s8>(const s8& t)
	{
		sprintf(sprintf_buffer, "%d", t);
		char* result = new char[strlen(sprintf_buffer)];
		strcpy(result, &sprintf_buffer[0]);
		return result;
	}

	template <>
	static const char* to_string<s16>(const s16& t)
	{
		sprintf(sprintf_buffer, "%d", t);
		char* result = new char[strlen(sprintf_buffer)];
		strcpy(result, &sprintf_buffer[0]);
		return result;
	}

	template <>
	static const char* to_string<s32>(const s32& t)
	{
		sprintf(sprintf_buffer, "%ld", t);
		char* result = new char[strlen(sprintf_buffer)];
		strcpy(result, &sprintf_buffer[0]);
		return result;
	}

	template <>
	static const char* to_string<s64>(const s64& t)
	{
		sprintf(sprintf_buffer, "%lld", t);
		char* result = new char[strlen(sprintf_buffer)];
		strcpy(result, &sprintf_buffer[0]);
		return result;
	}

	template <>
	static const char* to_string<u8>(const u8& t)
	{
		sprintf(sprintf_buffer, "%u", t);
		char* result = new char[strlen(sprintf_buffer)];
		strcpy(result, &sprintf_buffer[0]);
		return result;
	}
	template <>
	static const char* to_string<u16>(const u16& t)
	{
		sprintf(sprintf_buffer, "%u", t);
		char* result = new char[strlen(sprintf_buffer)];
		strcpy(result, &sprintf_buffer[0]);
		return result;
	}

	template <>
	static const char* to_string<u32>(const u32& t)
	{
		sprintf(sprintf_buffer, "%lu", t);
		char* result = new char[strlen(sprintf_buffer)];
		strcpy(result, &sprintf_buffer[0]);
		return result;
	}

	template <>
	static const char* to_string<u64>(const u64& t)
	{
		sprintf(sprintf_buffer, "%llu", t);
		char* result = new char[strlen(sprintf_buffer)];
		strcpy(result, &sprintf_buffer[0]);
		return result;
	}

	template <>
	static const char* to_string<long>(const long& t)
	{
		sprintf(sprintf_buffer, "%ld", t);
		char* result = new char[strlen(sprintf_buffer)];
		strcpy(result, &sprintf_buffer[0]);
		return result;
	}

	template <>
	static const char* to_string<unsigned long>(const unsigned long& t)
	{
		sprintf(sprintf_buffer, "%lu", t);
		char* result = new char[strlen(sprintf_buffer)];
		strcpy(result, &sprintf_buffer[0]);
		return result;
	}

	template <>
	static const char* to_string<float32>(const float32& t)
	{
		sprintf(sprintf_buffer, "%f", t);
		char* result = new char[strlen(sprintf_buffer)];
		strcpy(result, &sprintf_buffer[0]);
		return result;
	}

	template <>
	static const char* to_string<float64>(const float64& t)
	{
		sprintf(sprintf_buffer, "%f", t);
		char* result = new char[strlen(sprintf_buffer)];
		strcpy(result, &sprintf_buffer[0]);
		return result;
	}

	template <typename T>
	void PrintSequence_int(byte* buffer, s32& index, T&& t)
	{
		const byte* string = cast(const byte*) to_string<T>(t);
		s32 len = std::strlen(cast(const char*) string);
		memcpy(&buffer[index], string, len);
		index += len;
	}

	template <typename T, typename... Rest>
	void PrintSequence_int(byte* buffer, s32& index, T&& first, Rest&&... rest)
	{
		const byte* string = cast(const byte*) to_string<T>(first);
		s32 len = std::strlen(cast(const char*) string);
		memcpy(&buffer[index], string, len);
		index += len;
		if (sizeof...(Rest))
			PrintSequence_int(buffer, index, std::forward<Rest>(rest)...);
	}

	template <typename... Args>
	void PrintSequence(s32 level, Args... args)
	{
		byte buffer[1024 * 10];
		s32 index = 0;
		PrintSequence_int(buffer, index, std::forward<Args>(args)...);

		buffer[index] = 0;

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		switch (level)
		{
		case LOG_LEVEL_FATAL:
			SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case LOG_LEVEL_ERROR:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case LOG_LEVEL_WARN:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		}
		printf("%s", buffer);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	}
}

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
	#define LOG_INFO(...) cl::PrintSequence(LOG_LEVEL_INFO, __FUNCTION__, " [", __LINE__, "]: ", __VA_ARGS__)
#else
	#define LOG_INFO(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(...) cl::PrintSequence(LOG_LEVEL_WARN, __FUNCTION__, " [", __LINE__, "]: ", __VA_ARGS__)
#else
#define LOG_WARN(...)
#endif


#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(...) cl::PrintSequence(LOG_LEVEL_ERROR, __FUNCTION__, " [", __LINE__, "]: ", __VA_ARGS__)
#else
#define LOG_ERROR(...)
#endif


#if LOG_LEVEL >= LOG_LEVEL_FATAL
#define LOG_FATAL(...) cl::PrintSequence(LOG_LEVEL_FATAL, __FUNCTION__, " [", __LINE__, "]: ", __VA_ARGS__)
#else
#define LOG_FATAL(...)
#endif

#define ASSERT(x, ...) \
		if (!(x)) {\
			LOG_FATAL("!!!!!!!!!!!!!!!!!!!!!!!!!\n"); \
			LOG_FATAL("!   ASSERTION FAILED    !\n"); \
			LOG_FATAL("!!!!!!!!!!!!!!!!!!!!!!!!!\n"); \
			LOG_FATAL(__FILE__, ": ", __LINE__, "\n"); \
			LOG_FATAL("Failed condition: ", #x, "\n"); \
			LOG_FATAL(__VA_ARGS__, "\n"); \
			__debugbreak(); \
		}
