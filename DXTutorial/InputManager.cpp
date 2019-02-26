#pragma once

#include "InputManager.h"
#include <dinput.h>
#include "WriteLog.h"
#include "Assert.h"


InputManager::InputManager()
{
}


InputManager::~InputManager()
{
	ShutDown();
}


bool InputManager::StartUp(SampleWindow& window)
{
	if (!m_keyboard.StartUp(window))
	{
		return false;
	}

	if (!m_mouse.StartUp(window))
	{
		return false;
	}

	if (!m_gamepad.StartUp())
	{
		return false;
	}

	return true;
}


void InputManager::ShutDown()
{
	m_keyboard.ShutDown();
	m_mouse.ShutDown();
	m_gamepad.ShutDown();
}


void InputManager::UpdateAll()
{
	m_keyboard.Update();
	m_mouse.Update();
	m_gamepad.Update();
}


const Keyboard& InputManager::GetKeyboard() const
{
	return m_keyboard;
}


const Mouse& InputManager::GetMouse() const
{
	return m_mouse;
}


const Gamepad& InputManager::GetGamepad() const
{
	return m_gamepad;
}