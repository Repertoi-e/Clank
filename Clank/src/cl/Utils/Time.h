#pragma once

#include "cl/stdafx.h"

#include <Windows.h>

namespace cl {

	struct API Time
	{
		u64 Cycles;
		float64 Frequency;

		float32 Millis(void)
		{
			return cast(float32) (Cycles * Frequency) * 1000.0f;
		}

		float32 Seconds(void)
		{
			return cast(float32) (Cycles * Frequency);
		}
	};

	class API Timer
	{
	private:
		LARGE_INTEGER m_Start;
		float64 m_Frequency;
	public:
		Timer(void)
		{
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);
			m_Frequency = 1.0 / frequency.QuadPart;

			Reset();
		}

		void Reset(void)
		{
			QueryPerformanceCounter(&m_Start);
		}

		Time Elapsed(void)
		{
			Time time;
			
			LARGE_INTEGER current;
			QueryPerformanceCounter(&current);
			time.Cycles = current.QuadPart - m_Start.QuadPart;
			time.Frequency = m_Frequency;
			return time;
		}
	};

	class API DeltaTime
	{
	private:
		float32 m_Difference = 0;
		float32 m_LastTime;
	public:
		DeltaTime(float32 start)
			: m_LastTime(start)
		{
		}

		void Update(float32 currentTime)
		{
			m_Difference = currentTime - m_LastTime;
			m_LastTime = currentTime;
		}

		const Time Elapsed(void) const
		{
			Time time;
			time.Cycles = 1;
			time.Frequency = m_Difference * 0.001f;
			return time;
		}
	};
}