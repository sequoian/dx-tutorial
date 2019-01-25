#pragma once

enum MouseButtons
{
	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_04
};

class SampleWindow;


class Mouse
{
public:
	Mouse();
	~Mouse();
	bool StartUp(SampleWindow& window);
	void ShutDown();

	// call every frame
	bool Update();
	
	bool GetButtonState(MouseButtons button);
	void GetMovementDelta(int& x, int& y);
	void GetWheelDelta(int& wheel);
	
	// gets location in desktop coordinates
	void GetLocation(int& x, int& y);
	bool SetExclusiveMode(bool exclusive, SampleWindow& window);

private:
	bool AcquireMouse();

private:
	struct Impl;
	Impl* m_impl;
};