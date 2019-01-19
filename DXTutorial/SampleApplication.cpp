#include "SampleApplication.h"


SampleApplication::SampleApplication()
{
	initialized = false;
}


SampleApplication::~SampleApplication()
{
	ShutDown();
}


bool SampleApplication::StartUp()
{
	StartUpLogger();

	// make sure startup is only called once
	assert(!initialized);
	initialized = true;

	if (!m_graphics.StartUp())
	{
		return false;
	}

	if (!m_window.StartUp(&m_graphics))
	{
		return false;
	}

	ShowWindow(m_window.Window(), 1);

	return true;
}


void SampleApplication::ShutDown()
{
	m_window.ShutDown();

	m_graphics.ShutDown();

	ShutDownLogger();
}


void SampleApplication::Run()
{
	// Run the message loop.
	while (ProcessWindowMessages())
	{
		Update();
		Render();

		m_window.Present();
	}
}


void SampleApplication::Update()
{
}


void SampleApplication::Render()
{
}


bool SampleApplication::ProcessWindowMessages()
{
	MSG msg;
	bool keepRunning = true;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			keepRunning = false;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return keepRunning;
}