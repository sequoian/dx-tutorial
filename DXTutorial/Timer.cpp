#include "Timer.h"

#include <Windows.h>
#include "Assert.h"
#include "WriteLog.h"


void Timer::Start()
{
	ASSERT_VERBOSE(s_cyclesPerSecond != 0, "Timers must be initialized first");
	ASSERT(m_lastUpdateCycle == 0);

	m_lastUpdateCycle = GetHiResCounter();
	
}


void Timer::Update()
{
	ASSERT_VERBOSE(m_lastUpdateCycle != 0, "Timer was not started before Update was called");

	if (!m_isPaused)
	{
		// get current timestamp
		unsigned long long now = GetHiResCounter();

		// set delta time
		m_deltaTime = CyclesToSeconds(now - m_lastUpdateCycle);

		// account for long pauses
		if (m_deltaTime > 5.0)
		{
			m_deltaTime = 1.0 / 30.0;
		}

		// update lastUpdatedCycle
		m_lastUpdateCycle = now;

		// update timeElapsed
		m_timeElapsed += m_deltaTime * m_timeScale;
	}
}


float Timer::GetTime()
{
	return m_timeElapsed;
}


float Timer::GetDeltaTime()
{
	return m_deltaTime * m_timeScale;
}


float Timer::GetUnscaledDeltaTime()
{
	return m_deltaTime;
}


void Timer::Step()
{
	if (m_isPaused)
	{
		m_deltaTime = 1.0 / 30.0;
	}
}

void Timer::SetPaused(bool pause)
{
	m_isPaused = pause;
}


bool Timer::IsPaused() const
{
	return m_isPaused;
}


void Timer::SetTimeScale(float scale)
{
	m_timeScale = scale;
}


float Timer::GetTimeScale() const
{
	return m_timeScale;
}


/* Statics */

float Timer::s_cyclesPerSecond = 0;


void Timer::InitTimers()
{
	ASSERT(s_cyclesPerSecond == 0);

	s_cyclesPerSecond = (float)GetHiResFrequency();
}


unsigned long long Timer::GetHiResFrequency()
{
	LARGE_INTEGER frequency;
	if (!QueryPerformanceFrequency(&frequency))
	{
		DEBUG_ERROR("Failed to query performance frequency");
		return 0;
	}
	return frequency.QuadPart;
}


unsigned long long Timer::GetHiResCounter()
{
	LARGE_INTEGER counter;
	if (!QueryPerformanceCounter(&counter))
	{
		DEBUG_ERROR("Failed to query performance counter");
		return 0;
	}
	return counter.QuadPart;
}


float Timer::CyclesToSeconds(unsigned long long cycles)
{
	return (float)cycles / s_cyclesPerSecond;
}


unsigned long long Timer::SecondsToCycles(float seconds)
{
	return (unsigned long long)(seconds * s_cyclesPerSecond);
}