#include "Application.h"
#include "Assert.h"
#include "Timer.h"


Application::Application()
{
	initialized = false;
}


Application::~Application()
{
	ShutDown();
}


bool Application::StartUp()
{
	// make sure startup is only called once
	ASSERT(!initialized);
	initialized = true;

	if (!StartUpLogger())
	{
		return false;
	}

	Timer::InitTimers();

	if (!m_graphics.StartUp())
	{
		return false;
	}

	if (!m_window.StartUp(&m_graphics))
	{
		return false;
	}

	ShowWindow(m_window.Window(), 1);

	m_resourceManager.StartUp(m_graphics);
	m_physics.StartUp(&m_eventBus);
	m_timer.Start();
	m_inputManager.StartUp(m_window);
	m_entityManager.StartUp(10);

	return true;
}


void Application::ShutDown()
{
	m_inputManager.ShutDown();
	m_physics.ShutDown();
	m_window.ShutDown();
	m_eventBus.ShutDown();
	m_graphics.ShutDown();
	ShutDownLogger();
}


void Application::Run()
{
	// Run the message loop.
	while (ProcessWindowMessages())
	{
		Update();
		Render();

		m_window.Present();
	}
}


void Application::Update()
{
	m_timer.Update();
	m_inputManager.UpdateAll();
}


void Application::Render()
{
}


bool Application::ProcessWindowMessages()
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