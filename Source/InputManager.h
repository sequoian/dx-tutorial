#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"

class SampleWindow;

class InputManager
{
public:
	InputManager();
	~InputManager();
	bool StartUp(SampleWindow& window);
	void ShutDown();
	void UpdateAll();
	
	const Keyboard& GetKeyboard() const;
	const Mouse& GetMouse() const;
	const Gamepad& GetGamepad() const;

private:
	Mouse m_mouse;
	Keyboard m_keyboard;
	Gamepad m_gamepad;
};