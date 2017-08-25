#pragma once

#include "cl/stdafx.h"

#include "cl/Maths/maths.h"

#include <codecvt>
#include <string>

#define LOG_LEVEL_NOLOG -1
#define LOG_LEVEL_FATAL  0
#define LOG_LEVEL_ERROR	 1
#define LOG_LEVEL_WARN	 2
#define LOG_LEVEL_INFO	 3

namespace cl {

	static wchar to_wchar_buffer[1024 * 10];
	static wchar sprintf_buffer[1024 * 10];

	template <typename T>
	static const wchar* to_string(const T& t)
	{
		const char* c = typeid(T).name();

		mbstowcs(to_wchar_buffer, c, strlen(c) + 1);
		return to_wchar_buffer;
	}
	
	template <>
	static const wchar* to_string<wchar>(const wchar& t)
	{
		return &t;
	}

	template <>
	static const wchar* to_string<char*>(char* const& t)
	{
		if (!t)
			return L""; 
		
		mbstowcs(to_wchar_buffer, t, 2);
		return to_wchar_buffer;
	}

	template <>
	static const wchar* to_string<char>(const char& t)
	{
		mbstowcs(to_wchar_buffer, &t, 2);
		return to_wchar_buffer;
	}

	template <>
	static const wchar* to_string<String>(const String& t)
	{
		return t.c_str();
	}

	template <>
	static const wchar* to_string<const char*>(const char* const& t)
	{
		if (!t)
			return L"";

		mbstowcs(to_wchar_buffer, t, strlen(t) + 1);
		return to_wchar_buffer;
	}

	template <>
	static const wchar* to_string<wchar*>(wchar* const& t)
	{
		return t;
	}

	template <>
	static const wchar* to_string<wchar const*>(wchar const* const& t)
	{
		return t;
	}

	template <>
	static const wchar* to_string<bool>(const bool& t)
	{
		return t ? L"true" : L"false";
	}

	template <>
	static const wchar* to_string<s8>(const s8& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"%d", t);
		return sprintf_buffer;
	}

	template <>
	static const wchar* to_string<s16>(const s16& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"%d", t);
		return sprintf_buffer;
	}

	template <>
	static const wchar* to_string<s32>(const s32& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"%ld", t);
		return sprintf_buffer;
	}

	template <>
	static const wchar* to_string<s64>(const s64& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"%lld", t);
		return sprintf_buffer;
	}

	template <>
	static const wchar* to_string<u8>(const u8& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"%u", t);
		return sprintf_buffer;
	}
	template <>
	static const wchar* to_string<u16>(const u16& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"%u", t);
		return sprintf_buffer;
	}

	template <>
	static const wchar* to_string<u32>(const u32& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"%lu", t);
		return sprintf_buffer;
	}

	template <>
	static const wchar* to_string<u64>(const u64& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"%llu", t);
		return sprintf_buffer;
	}

	template <>
	static const wchar* to_string<long>(const long& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"%ld", t);
		return sprintf_buffer;
	}

	template <>
	static const wchar* to_string<unsigned long>(const unsigned long& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"%lu", t);
		return sprintf_buffer;
	}

	template <>
	static const wchar* to_string<float32>(const float32& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"%f", t);
		return sprintf_buffer;
	}

	template <>
	static const wchar* to_string<float64>(const float64& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"%f", t);
		return sprintf_buffer;
	}

	template <>
	static const wchar* to_string<vec2>(const vec2& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"vec2 { %f, %f }", t.x, t.y);
		return sprintf_buffer;
	}

	template <>
	static const wchar* to_string<vec3>(const vec3& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"vec3 { %f, %f, %f }", t.x, t.y, t.z);
		return sprintf_buffer;
	}

	template <>
	static const wchar* to_string<vec4>(const vec4& t)
	{
		swprintf(sprintf_buffer, 1024 * 10, L"vec4 { %f, %f, %f, %f }", t.x, t.y, t.z, t.w);
		return sprintf_buffer;
	}

	template <typename T>
	void print_sequence_internal(wchar* buffer, s32& index, T&& t)
	{
		const wchar* string = to_string<T>(t);
		s32 len = wcslen(string);
		wmemcpy(&buffer[index], string, len);
		index += len;
	}

	template <typename T, typename... Rest>
	void print_sequence_internal(wchar* buffer, s32& index, T&& first, Rest&&... rest)
	{
		const wchar* string = to_string<T>(first);
		s32 len = wcslen(string);
		wmemcpy(&buffer[index], string, len);
		index += len;
		if (sizeof...(Rest))
			print_sequence_internal(buffer, index, std::forward<Rest>(rest)...);
	}

	template <typename... Args>
	const wchar* sprint_sequence(Args... args)
	{
		wchar buffer[1024 * 10];
		s32 index = 0;
		print_sequence_internal(buffer, index, std::forward<Args>(args)...);

		buffer[index] = '\0';

		return buffer;
	}

	template <typename... Args>
	void print_sequence(s32 level, Args... args)
	{
		wchar buffer[1024 * 10];
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
		wprintf(L"%ls", buffer);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	}

	template <typename T>
	void sprint_internal(std::vector<const wchar*>& argsv, T&& t)
	{
		const wchar* ptr = to_string<T>(t);
		wchar* result = anew wchar[wcslen(ptr) + 1];
		wcscpy(result, ptr);
		argsv.push_back(result);
	}

	template <typename T, typename... Rest>
	void sprint_internal(std::vector<const wchar*>& argsv, T&& first, Rest&&... rest)
	{
		const wchar* ptr = to_string<T>(first);
		wchar* result = anew wchar[wcslen(ptr) + 1];
		wcscpy(result, ptr);
		argsv.push_back(result);
		if (sizeof...(Rest))
			sprint_internal(argsv, std::forward<Rest>(rest)...);
	}

	template <typename... Args>
	void print(Args... args)
	{
		wchar buffer[1024 * 10];
		s32 index = 0;
		sprint(buffer, &index, std::forward<Args>(args)...);
		wprintf(L"%ls", buffer);
	}

	template <typename... Args>
	void sprint(wchar* buffer, s32* indexPtr, Args... args)
	{
		s32* index = indexPtr;

		if (!index)
			index = anew s32(0);

		std::vector<const wchar*> argsv;
		sprint_internal(argsv, std::forward<Args>(args)...);

		const s32 args_count = argsv.size();

		if (args_count <= 0)
			return;

		s32 arg = 1;
		for (const wchar* ch = argsv[0]; *ch; ch++)
		{
			if (*ch == '%')
			{
				if (*(--ch) == '/')
				{
					const wchar* src = [&] {
						if (arg < 0 || arg >= args_count)
							return L"%";
						return argsv[arg++];
					}();

					s32 size = wcslen(src);
					wmemcpy(&buffer[(*index) - 1], src, size);
					(*index) += size - 1;
					ch++;

					continue;
				}
				ch++;
			}
			buffer[(*index)++] = *ch;
		} 
		buffer[*index] = '\0';

		for (s32 i = 0; i < args_count; i++)
			del[] argsv[i];
		
		if (indexPtr != index)
			del index;
	}

	static void SetLocale(s32 locale)
	{
		const char* l = setlocale(locale, "");
		print(!l ? "Locale not set\n\n" : "Locale set to /%\n\n", l);
	}
}

#ifndef LOG_LEVEL
	#ifdef _DEBUG
		#define LOG_LEVEL LOG_LEVEL_INFO
	#else
		#define LOG_LEVEL LOG_LEVEL_NOLOG
	#endif
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
