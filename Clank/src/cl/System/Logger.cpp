#include "cl/stdafx.h"
#include "Logger.h"

namespace cl {

	Logger& g_Logger = Logger::Instance();
	
	void Logger::Start(void)
	{
		m_LogLevel = INFO; 
		m_HConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
		Print("Starting Logging system...\n");

		SetLocale(LC_ALL);
	}

	void Logger::Shutdown(void)
	{
		Print(L"Shutting down Logging system...\n");
	}

	void Logger::SetLocale(s32 locale)
	{
		const char* l = setlocale(locale, "");
		Print(!l ? "Locale not set\n" : "Locale set to %\n", l);
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

		u32 size = u32(end - p);
		wmemcpy(&buffer.Data[buffer.Occupied], p, size);
		buffer.Occupied += size;
	}

	void print_u64_dec(u64 number, u32& index, bool printLeadingZeroes)
	{
		u64 divisor = 10000000000000000000u;

		bool allZeroes = true;

		while (divisor)
		{
			u64 place = number / divisor;
			number %= divisor;
			divisor /= 10;

			if (!printLeadingZeroes)
			{
				if (allZeroes)
				{
					if (!divisor || place != 0)
						allZeroes = false;
				}
				if (allZeroes)
					continue;
			}
			// m_Buffer[index++] = wchar(L'0' + place);
		}
	}

	void print_u64_hex(u64 number, u32& index, bool printLeadingZeroes)
	{
		u64 mask = 0xf000000000000000;

		bool allZeroes = true;

		for (u32 i = 0; i < 16; i++)
		{
			u64 place = number & mask;
			place >>= 60;
			number <<= 4;

			if (!printLeadingZeroes)
			{
				if (allZeroes)
				{
					if (i == 15 || place != 0)
						allZeroes = false;
				}
				if (allZeroes)
					continue;
			}
			// m_Buffer[index++] = [&]() -> wchar {
			// 	return place < 10 ? wchar(L'0' + place) : wchar(L'a' + place - 10);
			// }();
		}
	}

	void Logger::HandleArgument(StringBuffer& buffer, const PrintArg& arg)
	{
		void* value = arg.m_Value;

		switch (arg.m_Type)
		{
		case PrintArg::INTEGER:
		{
			FormatInt& integer = *cast(FormatInt*) value;
			switch (integer.m_Base)
			{
			case 10:
				if (integer.m_Negative)
					buffer.Append(L"-");
			default:
				PrintNumber(buffer, integer.m_IntegerValue, integer.m_Base);
				break;
			}

			if (arg.m_ShouldFree)
				del value;
			break;
		}
		case PrintArg::WSTRING:
			buffer.Append(cast(const wchar*) value);
			break;
		case PrintArg::STRING:
			buffer.Append(cast(const char*) value);
			break;
		}
	}

	void Logger::PrintInternal(StringBuffer& buffer, const PrintArg* args, u32 argc)
	{
		const PrintArg& first = args[0];

		// If there is only one argument,
		// then there is no need for formatting.
		if (argc == 1)
		{
			switch (first.m_Type)
			{
			case PrintArg::WSTRING:
				buffer.Append(cast(wchar*) first.m_Value);
				break;
			case PrintArg::STRING:
				buffer.Append(cast(char*) first.m_Value);
				break;
			}
			return;
		}

		wchar* format = NULLPTR;

		if (first.m_Type == PrintArg::STRING)
		{
			const char* str = cast(const char*) first.m_Value;
			wchar buffer[1024 * 10] = { 0 };
			mbstowcs(buffer, str, strlen(str) + 1);
			u32 len = wcslen(buffer);

			format = anew wchar[len];
			wmemcpy(format, buffer, len);
		}
		if (first.m_Type == PrintArg::WSTRING)
		{
			format = cast(wchar*) first.m_Value;
		}

		if (format == NULLPTR)
			return;

		u32 arg = 1;

		bool hasArgs = true;
		for (const wchar* it = format; *it; it++)
		{
			if (*it == L'%' && hasArgs)
			{
				HandleArgument(buffer, args[arg++]);

				if (arg + 1 > argc)
				{
					hasArgs = false;
					continue;
				}
				it++;
			}
			buffer.AppendChar(*it);
		}

		if (format != cast(wchar*) first.m_Value)
			del format;
	}

	void Logger::PrintSequenceInternal(StringBuffer& buffer, const PrintArg* args, u32 argc)
	{
		u32 index = 0;
		for (u32 i = 0; i < argc; i++)
			HandleArgument(buffer, args[i]);
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
}