#include "cl/stdafx.h"
#include "Logger.h"

#include <filesystem>
#include <cstddef>

namespace cl {

	Logger& g_Logger = Logger::Instance();

	void Logger::Start(void)
	{
		m_LogLevel = INFO; 
		m_HConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		Print(L"Starting Logging system...% % % %\n", 5, true, false, 4);

		SetLocale(LC_ALL);
	}

	void Logger::Shutdown(void)
	{
		Print(L"Shutting down Logging system...\n");
	}

	void Logger::SetLocale(s32 locale)
	{
		const char* l = setlocale(locale, "");
		//Print(!l ? "Locale not set\n" : "Locale set to %\n", l);
	}

	void Logger::SetLogLevel(LogLevel level)
	{
		m_LogLevel = level;
	}

	void Logger::PrintNumber(StringBuffer& buffer, u64 number, u64 base)
	{
		constexpr wchar* table = L"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_#";

		ASSERT(base >= 2, "Invalid base!");
		ASSERT(base <= 64, "Invalid base!");

		constexpr u32 MAX_OUTPUT_LEN = 64;

		wchar output[MAX_OUTPUT_LEN];

		wchar* end = &output[MAX_OUTPUT_LEN];
		wchar* p = end;

		while (number)
		{
			u64 place = number % base;
			number /= base;

			p -= 1;
			*p = table[place];
		}

		if (p == end)
		{
			p -= 1;
			*p = L'0';
		}

		buffer.AppendString(p, u32(end - p));
	}

	void Logger::HandleArgument(StringBuffer& buffer, const PrintArg& arg)
	{
		const std::any& value = arg.m_Value;

		switch (arg.m_Type)
		{
		case PrintArg::TypeEnum::INTEGER:
		{
			{
				FormatInt& formatInt = std::any_cast<FormatInt>(value);

				if (formatInt.m_Base == 10 && formatInt.m_Negative)
					buffer.AppendString(L"-");
				PrintNumber(buffer, formatInt.m_IntegerValue, formatInt.m_Base);
			}
			break;
		}
		case PrintArg::TypeEnum::FLOAT:
		{
			FormatFloat& formatFloat = std::any_cast<FormatFloat>(value);

			wchar temp[512] = { 0 };
			swprintf(temp, L"%*.*f", formatFloat.m_FieldWidth, formatFloat.m_Precision, formatFloat.m_FloatValue);
			buffer.AppendString(temp);
		}
		break;
		case PrintArg::TypeEnum::STRING:
		{
			SmartString& str = std::any_cast<SmartString>(value);
			buffer.AppendString(str.String);

			str.TryFree();
		}
		break;
		case PrintArg::TypeEnum::BOOL:
			buffer.AppendString(std::any_cast<bool>(value) ? L"true" : L"false");
			break;
		}
	}

	void Logger::PrintInternal(StringBuffer& buffer, const PrintArg* args, u32 argc)
	{
		const PrintArg& first = args[0];

		if (u32 arg = 1; first.m_Type == PrintArg::TypeEnum::STRING)
		{
			SmartString& str = std::any_cast<SmartString>(first.m_Value);

			wchar* s = str.String;
			wchar* t = s;

			while (*t)
			{
				if (*t == L'%')
				{
					if (arg < argc)
					{
						buffer.AppendString(s, u32(t - s));
						HandleArgument(buffer, args[arg]);

						t++;
						s = t;

						arg++;
					}
				}
				t++;
			}
			buffer.AppendString(s, u32(t - s));
			str.TryFree();
		}
	}

	void Logger::PrintSequenceInternal(StringBuffer& buffer, const PrintArg* args, u32 argc)
	{
		for (u32 arg = 0; arg < argc; arg++)
			HandleArgument(buffer, args[arg]);
	}

	void Logger::PrintColored(StringBuffer& buffer, LogLevel level)
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
		wprintf(buffer.Data);
		SetConsoleTextAttribute(m_HConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	}

	void PrintArg::HandleString(const char ch)
	{
		StringBuffer buffer;
		buffer.AppendChar(ch);

		m_Value = std::any(SmartString(buffer.Data, true));
	}

	void PrintArg::HandleString(const char* str)
	{
		StringBuffer buffer;
		buffer.AppendString(str);

		m_Value = std::any(SmartString(buffer.Data, true));
	}

	void PrintArg::HandlePrinter(const IPrintable& printable)
	{
		StringBuffer buffer;
		printable.Print(buffer);

		m_Value = std::any(SmartString(buffer.Data, true));
	}
}