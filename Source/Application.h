#pragma once

#include <stdio.h>
#include "Graphics.h"
#include "SampleWindow.h"
#include "EventBus.h"
#include "ResourceManager.h"
#include "Physics.h"
#include "Timer.h"
#include "InputManager.h"
#include "EntityManager.h"

class Application 
{
protected:
	Graphics m_graphics;
	Timer m_timer;
	ResourceManager m_resourceManager;
	InputManager m_inputManager;
	EntityManager m_entityManager;
	Physics m_physics;
	EventBus m_eventBus;
	SampleWindow m_window;
	bool initialized;

protected:
	bool ProcessWindowMessages();

public:
	Application();
	~Application();
	virtual bool StartUp();
	virtual void ShutDown();
	virtual void Run();
	virtual void Update();
	virtual void Render();
};