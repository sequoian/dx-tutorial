#include "Mouse.h"

#include <dinput.h>
#include "WriteLog.h"
#include "Assert.h"
#include "SampleWindow.h"


struct Mouse::Impl
{
	IDirectInput8* directInput;
	IDirectInputDevice8* mouse;
	DIMOUSESTATE mouseState;
	bool acquired;
	bool exclusiveMode;
};


Mouse::Mouse()
{
	m_impl = nullptr;
}


Mouse::~Mouse()
{
	ShutDown();
}


bool Mouse::StartUp(SampleWindow& window)
{
	ASSERT(m_impl == nullptr);
	m_impl = new Impl;
	m_impl->directInput = nullptr;
	m_impl->mouse = nullptr;
	m_impl->acquired = false;
	m_impl->exclusiveMode = false;

	HRESULT hr;
	hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_impl->directInput, NULL);
	if (hr != S_OK)
	{
		DEBUG_ERROR("Failed to create Direct Input");
		return false;
	}

	hr = m_impl->directInput->CreateDevice(GUID_SysMouse, &m_impl->mouse, NULL);
	if (hr != S_OK)
	{
		DEBUG_PRINT("Failed to create mouse device");
		return false;
	}

	hr = m_impl->mouse->SetDataFormat(&c_dfDIMouse);
	if (hr != S_OK)
	{
		DEBUG_PRINT("Failed to set data format for mouse");
		return false;
	}

	DWORD level = m_impl->exclusiveMode ? DISCL_FOREGROUND | DISCL_EXCLUSIVE : DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;
	hr = m_impl->mouse->SetCooperativeLevel(window.Window(), level);
	if (hr != S_OK)
	{
		DEBUG_PRINT("Failed to set cooperative level for mouse");
		return false;
	}

	return true;
}


void Mouse::ShutDown()
{
	if (m_impl)
	{
		if (m_impl->mouse)
		{
			m_impl->mouse->Unacquire();
			m_impl->mouse->Release();
			m_impl->mouse = nullptr;
		}

		if (m_impl->directInput)
		{
			m_impl->directInput->Release();
			m_impl->directInput = nullptr;
		}
	}

	delete m_impl;
	m_impl = nullptr;
}


bool Mouse::Update()
{
	if (!m_impl->acquired)
	{
		if (!AcquireMouse())
		{
			return false;
		}
	}

	HRESULT hr = m_impl->mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_impl->mouseState);
	if (hr != S_OK)
	{
		m_impl->acquired = false;
		return false;
	}

	return true;
}


bool Mouse::GetButtonState(MouseButtons button)
{
	// Size of DIMOUSESTATE rgbButtons. Use DIMOUSESTATE2 for more buttons.
	static int numButtons = 4; 

	ASSERT_VERBOSE(button < numButtons, "Mouse button not recognized");

	return (m_impl->mouseState.rgbButtons[button] & 0x80) != 0;
}


void Mouse::GetMovementDelta(int& x, int& y)
{
	x = m_impl->mouseState.lX;
	y = m_impl->mouseState.lY;
	
}


void Mouse::GetWheelDelta(int& wheel)
{
	wheel = m_impl->mouseState.lZ;
}


void Mouse::GetLocation(int& x, int& y)
{
	POINT p;
	GetCursorPos(&p);

	x = p.x;
	y = p.y;
}


bool Mouse::AcquireMouse()
{
	HRESULT hr = m_impl->mouse->Acquire();
	if (hr != S_OK)
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			DEBUG_ERROR("Failed to acquire mouse");
		}

		m_impl->acquired = false;
		return false;
	}

	m_impl->acquired = true;
	return true;
}