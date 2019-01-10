#pragma once

#include "Graphics.h"
#include "SampleWindow.h"

class SampleApplication 
{
protected:
	Graphics m_graphics;
	SampleWindow m_window;

protected:
	bool ProcessWindowMessages();

public:
	SampleApplication();
	~SampleApplication();
	void StartUp();
	void ShutDown();
	void Run();
	virtual void Update();
	virtual void Render();
};