#pragma once

#include "cl/stdafx.h"

#include "cl/Memory/Vector.h"

#include "cl/Utils/ISystem.h"
#include "cl/Utils/Singleton.h"

namespace cl {

#ifdef ERROR
	#undef ERROR
#endif

	class FormatInt
	{
	public:
		friend class Logger;
	private:
		u32 m_Base;

		u64 m_IntegerValue;
		bool m_Negative;
	public:
		template <typename T>
		FormatInt(T&& t, const u32 base = Base(10))
			: m_Base(base)
		{
			if (Type<T>::Integer)
			{
				if (Type<T>::Signed)
				{
					m_IntegerValue = cast(u64) (t < 0 ? -cast(s64) t : t);
					m_Negative = t < 0;
				}
				else
					m_IntegerValue = t;
			}
		}
	};

	class PrintArg
	{
	public:
		enum ArgType
		{
			INTEGER, WSTRING, STRING
		};
		ArgType m_Type;

		void* m_Value;
		u32 m_ShouldFree = false;
	public:
		template <typename T>
		void HandleInt(T&& t)
		{
			m_Type = INTEGER;
			m_Value = anew FormatInt(t, Base(10));
			m_ShouldFree = true;
		}

#define CTOR_INT(type) PrintArg(type i) { HandleInt(i); }

		CTOR_INT(s8)
		CTOR_INT(s16)
		CTOR_INT(s32)
		CTOR_INT(s64)
		CTOR_INT(u8)
		CTOR_INT(u16)
		CTOR_INT(u32)
		CTOR_INT(u64)

#define CTOR_TYPE(type, enumType, value) \
			PrintArg(type& t) { m_Type = enumType; m_Value = cast(void*) value; } \
			PrintArg(const type& t) { m_Type = enumType; m_Value = cast(void*) value; } \
			PrintArg(type&& t) { m_Type = enumType; m_Value = cast(void*) value; } \
			PrintArg(const type&& t) { m_Type = enumType; m_Value = cast(void*) value; } \

		CTOR_TYPE(String, WSTRING, t.c_str())
		CTOR_TYPE(wchar, WSTRING, &t)
		CTOR_TYPE(wchar* const, WSTRING, t)
		CTOR_TYPE(char, STRING, &t)
		CTOR_TYPE(char* const, STRING, t)

		CTOR_TYPE(FormatInt, INTEGER, &t)
		CTOR_TYPE(FormatInt* const, INTEGER, &t)
	};

	enum LogLevel : u8
	{
		NONE = 0,
		FATAL,
		ERROR,
		WARN,
		INFO
	};

	class Logger : public Singleton<Logger>, ISystem
	{
	private:
		LogLevel m_LogLevel;

		HANDLE m_HConsole;

		wchar m_Buffer[1024 * 10];
	public:
		void Start(void) override;
		void Shutdown(void) override;

		void SetLocale(s32 locale);
		void SetLogLevel(LogLevel level);
	private:
		void print_colored(LogLevel level)
		{
			switch (level)
			{
			case FATAL:
				SetConsoleTextAttribute(m_HConsole, BACKGROUND_RED | BACKGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;
			case ERROR:
				SetConsoleTextAttribute(m_HConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
				break;
			case WARN:
				SetConsoleTextAttribute(m_HConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				break;
			}
			wprintf(m_Buffer);
			SetConsoleTextAttribute(m_HConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		}
	public:
		template <typename... Args>
		void print(const Args&... args) 
		{
			if (m_LogLevel >= INFO)
			{
				PrintArg arg_array[] = { args... };
				print_internal(arg_array, sizeof...(Args));
				print_colored(INFO);
			}
		}
	
		template <typename... Args>
		void print(LogLevel level, const Args&... args)
		{
			if (m_LogLevel >= level)
			{
				PrintArg arg_array[] = { args... };
				print_internal(arg_array, sizeof...(Args));
				print_colored(level);
			}
		}

		template <typename... Args>
		void print_sequence(const Args&... args)
		{
			if (m_LogLevel >= INFO)
			{
				PrintArg arg_array[] = { args... };
				print_sequence_internal(arg_array, sizeof...(Args));
				print_colored(INFO);
			}
		}

		template <typename... Args>
		void print_sequence(LogLevel level, const Args&... args)
		{
			if (m_LogLevel >= level)
			{
				PrintArg arg_array[] = { args... };
				print_sequence_internal(arg_array, sizeof...(Args));
				print_colored(level);
			}
		}

		template <typename... Args>
		String sprint(const Args&... args)
		{
			PrintArg arg_array[] = { args... };
			print_internal(arg_array, sizeof...(Args));
			return String(m_Buffer);
		}
	private:
		void print_string(const wchar* wstr, u32& index);
		void print_string(const char* str, u32& index);
		void print_u64(u64 number, u64 base, u32& index);

		void handle_arg(const PrintArg& arg, void* value, u32& index);
		
		void print_internal(const PrintArg* args, u32 argc);
		void print_sequence_internal(const PrintArg* args, u32 argc);
	};

	extern Logger& g_Logger;
}

#ifdef _DEBUG
	#define DEBUG_LOG(level, ...) g_Logger.print_sequence(level, __FUNCTION__, " [", __LINE__, "]: ", __VA_ARGS__)

	#define ASSERT(x, ...) \
		if (!(x)) {\
			DEBUG_LOG(FATAL, "*************************\n"); \
			DEBUG_LOG(FATAL, "    ASSERTION FAILED		\n"); \
			DEBUG_LOG(FATAL, "*************************\n"); \
			DEBUG_LOG(FATAL, __FILE__, ": ", __LINE__, "\n"); \
			DEBUG_LOG(FATAL, "Condition: ", #x, "\n"); \
			DEBUG_LOG(FATAL, __VA_ARGS__, "\n"); \
			__debugbreak(); \
		}
#else
	#define DEBUG_LOG(level, ...)

	#define ASSERT(x, ...)
#endif