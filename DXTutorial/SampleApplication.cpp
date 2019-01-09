#include "SampleApplication.h"


void SampleApplication::StartUp()
{
	m_graphics = Graphics();
	m_graphics.StartUp();

	m_window = SampleWindow();
	m_window.StartUp(&m_graphics);

	ShowWindow(m_window.Window(), 1);
}


void SampleApplication::ShutDown()
{
	m_window.ShutDown();

	m_graphics.ShutDown();
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