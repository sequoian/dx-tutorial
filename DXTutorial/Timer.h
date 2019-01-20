# pragma once

class Timer
{
public:

	// Start the timer
	void Start();

	// Call every frame
	void Update();

	// Get time passed in seconds since the timer started
	float GetTime();

	// Get time passed in seconds since last update
	float GetDeltaTime();

	// Get delta time, unaffected by the time scale
	float GetUnscaledDeltaTime();

	// Advance time by 1/30th of a second (if paused)
	void Step();

	void SetPaused(bool pause);
	bool IsPaused() const;
	void SetTimeScale(float scale);
	float GetTimeScale() const;

	// Call when the game first starts up
	static void InitTimers();

private:

	static unsigned long long GetHiResFrequency();
	static unsigned long long GetHiResCounter();
	static float CyclesToSeconds(unsigned long long cycles);
	static unsigned long long SecondsToCycles(float seconds);

private:

	unsigned long long m_lastUpdateCycle = 0;
	float m_timeElapsed = 0;
	float m_deltaTime = 0;
	float m_timeScale = 1;
	bool m_isPaused = false;

	static float s_cyclesPerSecond;
};