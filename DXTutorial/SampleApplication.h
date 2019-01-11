#pragma once

#include <stdio.h>
#include "Graphics.h"
#include "SampleWindow.h"

class SampleApplication 
{
protected:
	Graphics m_graphics;
	SampleWindow m_window;
	bool initialized;

protected:
	bool ProcessWindowMessages();

public:
	SampleApplication();
	~SampleApplication();
	bool StartUp();
	void ShutDown();
	void Run();
	virtual void Update();
	virtual void Render();
};