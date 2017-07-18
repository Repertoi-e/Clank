#pragma once

#include "cl/stdafx.h"

#define LOG_LEVEL_FATAL  0
#define LOG_LEVEL_ERROR	 1
#define LOG_LEVEL_WARN	 2
#define LOG_LEVEL_INFO	 3

namespace cl {

	//@Incomplete: This whole file should support unicode. For now only ASCII

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
	static const char* to_string<unsigned char*>(unsigned char* const& t)
	{
		return cast(const char*) t;
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
		return sprintf_buffer;
	}

	template <>
	static const char* to_string<s16>(const s16& t)
	{
		sprintf(sprintf_buffer, "%d", t);
		return sprintf_buffer;
	}

	template <>
	static const char* to_string<s32>(const s32& t)
	{
		sprintf(sprintf_buffer, "%ld", t);
		return sprintf_buffer;
	}

	template <>
	static const char* to_string<s64>(const s64& t)
	{
		sprintf(sprintf_buffer, "%lld", t);
		return sprintf_buffer;
	}

	template <>
	static const char* to_string<u8>(const u8& t)
	{
		sprintf(sprintf_buffer, "%u", t);
		return sprintf_buffer;
	}
	template <>
	static const char* to_string<u16>(const u16& t)
	{
		sprintf(sprintf_buffer, "%u", t);
		return sprintf_buffer;
	}

	template <>
	static const char* to_string<u32>(const u32& t)
	{
		sprintf(sprintf_buffer, "%lu", t);
		return sprintf_buffer;
	}

	template <>
	static const char* to_string<u64>(const u64& t)
	{
		sprintf(sprintf_buffer, "%llu", t);
		return sprintf_buffer;
	}

	template <>
	static const char* to_string<long>(const long& t)
	{
		sprintf(sprintf_buffer, "%ld", t);
		return sprintf_buffer;
	}

	template <>
	static const char* to_string<unsigned long>(const unsigned long& t)
	{
		sprintf(sprintf_buffer, "%lu", t);
		return sprintf_buffer;
	}

	template <>
	static const char* to_string<float32>(const float32& t)
	{
		sprintf(sprintf_buffer, "%f", t);
		return sprintf_buffer;
	}

	template <>
	static const char* to_string<float64>(const float64& t)
	{
		sprintf(sprintf_buffer, "%f", t);
		return sprintf_buffer;
	}

	template <typename T>
	void print_sequence_internal(byte* buffer, s32& index, T&& t)
	{
		const char* string = to_string<T>(t);
		s32 len = strlen(string);
		memcpy(&buffer[index], string, len);
		index += len;
	}

	template <typename T, typename... Rest>
	void print_sequence_internal(byte* buffer, s32& index, T&& first, Rest&&... rest)
	{
		const char* string = to_string<T>(first);
		s32 len = std::strlen(string);
		memcpy(&buffer[index], string, len);
		index += len;
		if (sizeof...(Rest))
			print_sequence_internal(buffer, index, std::forward<Rest>(rest)...);
	}

	template <typename... Args>
	const char* sprint_sequence(Args... args)
	{
		byte buffer[1024 * 10];
		s32 index = 0;
		print_sequence_internal(buffer, index, std::forward<Args>(args)...);

		buffer[index] = '\0';

		return cast(const char*) buffer;
	}

	template <typename... Args>
	void print_sequence(s32 level, Args... args)
	{
		byte buffer[1024 * 10];
		s32 index = 0;
		print_sequence_internal(buffer, index, std::forward<Args>(args)...);

		buffer[index] = '\0';

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

	template <typename T>
	void print_internal(std::vector<const char*>& argsv, T&& t)
	{
		const char* ptr = to_string<T>(t);
		char* result = new char[strlen(ptr) + 1];
		strcpy(result, ptr);
		argsv.push_back(result);
	}

	template <typename T, typename... Rest>
	void print_internal(std::vector<const char*>& argsv, T&& first, Rest&&... rest)
	{
		const char* ptr = to_string<T>(first);
		char* result = new char[strlen(ptr) + 1];
		strcpy(result, ptr);
		argsv.push_back(result);
		if (sizeof...(Rest))
			print_internal(argsv, std::forward<Rest>(rest)...);
	}

	template <typename... Args>
	void print(Args... args)
	{
		char buffer[1024 * 10];
		s32 index = 0;
		sprint(cast(byte*) buffer, index, std::forward<Args>(args)...);
		printf("%s", buffer);
	}

	//@Cleanup: Having to pass a reference to a s32 for buffer indexing is
	// annoying and rarely used but it is there for robustness. Make it
	// not obligatory.
	template <typename... Args>
	void sprint(byte* buffer, s32& index, Args... args)
	{
		std::vector<const char*> argsv;
		print_internal(argsv, std::forward<Args>(args)...);

		s32 args_count = argsv.size();

		if (args_count <= 0)
			return;

		s32 arg = 1;
		for (const char* ch = argsv[0]; *ch; ch++)
		{
			if (*ch == '%')
			{
				if (*(--ch) == '/')
				{
					const char* src = [&] {
						if (arg < 0 || arg >= args_count)
							return "%";
						return argsv[arg++];
					}();

					s32 size = strlen(src);
					memcpy(&buffer[index - 1], cast(void*) src, size);
					index += size - 1;
					ch++;

					continue;
				}
				ch++;
			}
			buffer[index++] = *ch;
		} 
		buffer[index] = '\0';

		for (u32 i = 0; i < args_count; i++)
			delete[] argsv[i];
	}
}

//@Incomplete: These macros call the print_sequence procedure. What if you
// want to log by formatting a string with arguments instead of specifying
// sequence?..

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_INFO(...) cl::print_sequence(LOG_LEVEL_INFO, __FUNCTION__, " [", __LINE__, "]: ", __VA_ARGS__)
#else
#define LOG_INFO(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(...) cl::print_sequence(LOG_LEVEL_WARN, __FUNCTION__, " [", __LINE__, "]: ", __VA_ARGS__)
#else
#define LOG_WARN(...)
#endif


#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(...) cl::print_sequence(LOG_LEVEL_ERROR, __FUNCTION__, " [", __LINE__, "]: ", __VA_ARGS__)
#else
#define LOG_ERROR(...)
#endif


#if LOG_LEVEL >= LOG_LEVEL_FATAL
#define LOG_FATAL(...) cl::print_sequence(LOG_LEVEL_FATAL, __FUNCTION__, " [", __LINE__, "]: ", __VA_ARGS__)
#else
#define LOG_FATAL(...)
#endif

#define ASSERT(x, ...) \
		if (!(x)) {\
			LOG_FATAL("*************************\n"); \
			LOG_FATAL("    ASSERTION FAILED		\n"); \
			LOG_FATAL("*************************\n"); \
			LOG_FATAL(__FILE__, ": ", __LINE__, "\n"); \
			LOG_FATAL("Condition: ", #x, "\n"); \
			LOG_FATAL(__VA_ARGS__, "\n"); \
			__debugbreak(); \
		}
