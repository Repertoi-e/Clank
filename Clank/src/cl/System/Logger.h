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
		friend class PrintArg;
	private:
		u32 m_Base;
		u32 m_Negative;
		
		u64 m_IntegerValue;
	public:
		template <typename T>
		FormatInt(T&& t, const u32 base = 10)
			: m_Base(base)
		{
			if constexpr(Type<STRIP_CVREF(T)>::Signed)
			{
				m_IntegerValue = cast(u64) (t < 0 ? -cast(s64)t : t);
				m_Negative = t < 0;
			}
			else
				m_IntegerValue = t;
		}
	};

	class FormatFloat
	{
	public:
		friend class Logger;
		friend class PrintArg;
	private:
		s32 m_Precision;
		s32 m_FieldWidth;
		
		float64 m_FloatValue;
	public:
		template <typename T>
		FormatFloat(T&& t, const s32 precision = -1, const s32 fieldWidth = -1)
			: m_Precision(precision), m_FieldWidth(fieldWidth)
		{
			m_FloatValue = cast(float64) t;
		}
	};

	class IPrintable
	{
	public:
		virtual void Print(StringBuffer& buffer) const = 0;
	};

	class PrintArg
	{
	public:
		enum TypeEnum
		{
			INTEGER = 0,
			FLOAT,
			STRING,
			BOOL
		};
		TypeEnum m_Type;

		std::any m_Value;
	public:
#define CTOR_TYPE(type, enumType, body) \
		PrintArg(type& t) { m_Type = enumType; body; } \
		PrintArg(const type& t) { m_Type = enumType; body; } \
		PrintArg(type&& t) { m_Type = enumType; body; } \
		PrintArg(const type&& t) { m_Type = enumType; body; } \

		CTOR_TYPE(s8, INTEGER, m_Value =  { FormatInt(t) })
		CTOR_TYPE(s16, INTEGER, m_Value = { FormatInt(t) })
		CTOR_TYPE(s32, INTEGER, m_Value = { FormatInt(t) })
		CTOR_TYPE(s64, INTEGER, m_Value = { FormatInt(t) })
		CTOR_TYPE(u8, INTEGER, m_Value =  { FormatInt(t) })
		CTOR_TYPE(u16, INTEGER, m_Value = { FormatInt(t) })
		CTOR_TYPE(u32, INTEGER, m_Value = { FormatInt(t) })
		CTOR_TYPE(u64, INTEGER, m_Value = { FormatInt(t) })
		CTOR_TYPE(FormatInt, INTEGER, m_Value = { CopyFInt(t) })

		CTOR_TYPE(float32, FLOAT, m_Value = { FormatFloat(t) })
		CTOR_TYPE(float64, FLOAT, m_Value = { FormatFloat(t) })
		CTOR_TYPE(FormatFloat, FLOAT, m_Value = { CopyFFloat(t) } )

		CTOR_TYPE(String, STRING, m_Value = { SmartString(t.c_str(), false) })
		CTOR_TYPE(wchar, STRING, m_Value = { SmartString(&t, false) })
		CTOR_TYPE(wchar* const, STRING, m_Value = { SmartString(t, false) })

		CTOR_TYPE(char, STRING, HandleString(t))
		CTOR_TYPE(char* const, STRING, HandleString(t))

		CTOR_TYPE(bool, BOOL, m_Value = { t })

		CTOR_TYPE(IPrintable, STRING, HandlePrinter(t))
	private:
		FormatInt CopyFInt(const FormatInt& fint) { return fint; }
		FormatFloat CopyFFloat(const FormatFloat& ffloat) { return ffloat; }

		void HandleString(const char ch);
		void HandleString(const char* str);

		void HandlePrinter(const IPrintable& printable);
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
	public:
		void Start(void) override;
		void Shutdown(void) override;

		void SetLocale(s32 locale);
		void SetLogLevel(LogLevel level);
	public:
		template <typename... Args>
		void Print(const Args&... args) 
		{
			if (m_LogLevel >= INFO)
			{
				StringBuffer buffer;

				PrintArg arg_array[] = { args... };
				PrintInternal(buffer, arg_array, sizeof...(Args));
				PrintColored(buffer, INFO);
			}
		}

		template <typename... Args>
		void Print(LogLevel level, const Args&... args)
		{
			if (m_LogLevel >= level)
			{
				StringBuffer buffer;

				PrintArg arg_array[] = { args... };
				PrintInternal(buffer, arg_array, sizeof...(Args));
				PrintColored(buffer, level);
			}
		}

		template <typename... Args>
		void PrintSequence(const Args&... args)
		{
			if (m_LogLevel >= INFO)
			{
				StringBuffer buffer;

				PrintArg arg_array[] = { args... };
				PrintSequenceInternal(buffer, arg_array, sizeof...(Args));
				PrintColored(buffer, INFO);
			}
		}

		template <typename... Args>
		void PrintSequence(LogLevel level, const Args&... args)
		{
			if (m_LogLevel >= level)
			{
				StringBuffer buffer;

				PrintArg arg_array[] = { args... };
				PrintSequenceInternal(buffer, arg_array, sizeof...(Args));
				PrintColored(buffer, level);
			}
		}

		template <typename... Args>
		String StringPrint(const Args&... args)
		{
			StringBuffer buffer;

			PrintArg arg_array[] = { args... };
			PrintInternal(buffer, arg_array, sizeof...(Args));

			return String(buffer.Data);
		}

		template <typename... Args>
		String StringPrintSequence(const Args&... args)
		{
			StringBuffer buffer;

			PrintArg arg_array[] = { args... };
			PrintInternal(buffer, arg_array, sizeof...(Args));

			return String(buffer.Data);
		}
	private:
		void PrintNumber(StringBuffer& buffer, u64 number, u64 base);

		void HandleArgument(StringBuffer& buffer, const PrintArg& arg);

		void PrintInternal(StringBuffer& buffer, const PrintArg* args, u32 argc);
		void PrintSequenceInternal(StringBuffer& buffer, const PrintArg* args, u32 argc);

		void PrintColored(StringBuffer& buffer, LogLevel level);
	};

	extern Logger& g_Logger;
}

#ifdef _DEBUG
	#define DEBUG_LOG(level, ...) g_Logger.PrintSequence(level, __FUNCTION__, " [", __LINE__, "]: ", __VA_ARGS__)

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