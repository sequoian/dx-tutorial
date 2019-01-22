#include "Gamepad.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <Xinput.h>
#include <math.h>
#include "WriteLog.h"
#include "Assert.h"

// Hide private implementation
struct Gamepad::Impl
{
	XINPUT_STATE m_state;
	int m_controllerId;
	float m_deadzone;
};


Gamepad::Gamepad()
{
}


Gamepad::~Gamepad()
{
	ShutDown();
}


bool Gamepad::StartUp()
{
	ASSERT(m_impl == nullptr);

	m_impl = new Impl;
	m_impl->m_deadzone = 0.2f;
	m_impl->m_controllerId = -1;

	return true;
}


void Gamepad::ShutDown()
{
	if (m_impl != nullptr)
	{
		delete m_impl;
		m_impl = nullptr;
	}
}


bool Gamepad::Update()
{
	// Note: overkill to check connection every frame?
	if (m_impl->m_controllerId == -1)
	{
		if (!CheckConnection())
		{
			return false;
		}
	}

	ZeroMemory(&m_impl->m_state, sizeof(XINPUT_STATE));
	if (XInputGetState(m_impl->m_controllerId, &m_impl->m_state) != ERROR_SUCCESS)
	{
		m_impl->m_controllerId = -1;
		return false;
	}

	return true;
}


bool Gamepad::GetButtonState(GamepadButtons button)
{
	static const unsigned int s_buttons[] = {
		XINPUT_GAMEPAD_DPAD_UP,
		XINPUT_GAMEPAD_DPAD_DOWN,
		XINPUT_GAMEPAD_DPAD_LEFT,
		XINPUT_GAMEPAD_DPAD_RIGHT,
		XINPUT_GAMEPAD_START,
		XINPUT_GAMEPAD_BACK,
		XINPUT_GAMEPAD_LEFT_THUMB,
		XINPUT_GAMEPAD_RIGHT_THUMB,
		XINPUT_GAMEPAD_LEFT_SHOULDER,
		XINPUT_GAMEPAD_RIGHT_SHOULDER,
		XINPUT_GAMEPAD_A,
		XINPUT_GAMEPAD_B,
		XINPUT_GAMEPAD_X,
		XINPUT_GAMEPAD_Y
	};
	static const unsigned int s_numButtons = sizeof(s_buttons) / sizeof(s_buttons[0]);

	ASSERT_VERBOSE(button < s_numButtons, "Gamepad button not recognized");

	return (m_impl->m_state.Gamepad.wButtons & s_buttons[button]) != 0;
}


float Gamepad::GetAxisState(GamepadAxes axis)
{
	float axisState = 0;

	switch (axis)
	{
		case LEFT_THUMB_X:
			axisState = NormalizeThumbstick(m_impl->m_state.Gamepad.sThumbLX);
			break;
		case LEFT_THUMB_Y:
			axisState = NormalizeThumbstick(m_impl->m_state.Gamepad.sThumbLY);
			break;
		case RIGHT_THUMB_X:
			axisState = NormalizeThumbstick(m_impl->m_state.Gamepad.sThumbRX);
			break;
		case RIGHT_THUMB_Y:
			axisState = NormalizeThumbstick(m_impl->m_state.Gamepad.sThumbRY);
			break;
		case LEFT_TRIGGER: 
			axisState = (float)m_impl->m_state.Gamepad.bLeftTrigger / 255;
			break;
		case RIGHT_TRIGGER: 
			axisState = (float)m_impl->m_state.Gamepad.bRightTrigger / 255;
			break;
		default:
			ASSERT_VERBOSE(0, "Gamepad axis not recognized");
			break;
	}

	return axisState;
}


XMVECTOR Gamepad::GetRightThumbstickVector()
{
	float normX = fmaxf(-1, (float)m_impl->m_state.Gamepad.sThumbRX / 32767);
	float normY = fmaxf(-1, (float)m_impl->m_state.Gamepad.sThumbRY / 32767);
	XMVECTOR vector = XMVectorSet(normX, normY, 0, 0);

	return NormalizeVector(vector);
}


XMVECTOR Gamepad::GetLeftThumbstickVector()
{
	float normX = fmaxf(-1, (float)m_impl->m_state.Gamepad.sThumbLX / 32767);
	float normY = fmaxf(-1, (float)m_impl->m_state.Gamepad.sThumbLY / 32767);
	XMVECTOR vector = XMVectorSet(normX, normY, 0, 0);

	return NormalizeVector(vector);
}


void Gamepad::SetDeadzone(float deadzone)
{
	m_impl->m_deadzone = deadzone;
}


float Gamepad::GetDeadzone() const
{
	return m_impl->m_deadzone;
}


int Gamepad::GetPort()
{
	return m_impl->m_controllerId + 1;
}


bool Gamepad::IsConnected()
{
	return m_impl->m_controllerId != -1;
}


bool Gamepad::CheckConnection()
{
	int controllerId = -1;

	for (DWORD i = 0; i < XUSER_MAX_COUNT && controllerId == -1; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		if (XInputGetState(i, &state) == ERROR_SUCCESS)
		{
			controllerId = i;
		}
	}

	m_impl->m_controllerId = controllerId;

	return controllerId != -1;
}


float Gamepad::NormalizeThumbstick(int axis)
{
	float normal = fmaxf(-1, (float)axis / 32767);
	float thumbstick = (fabs(normal) < m_impl->m_deadzone ? 0 : (fabs(normal) - m_impl->m_deadzone) * (normal / fabs(normal)));
	if (m_impl->m_deadzone > 0)
	{
		thumbstick /= 1 - m_impl->m_deadzone;
	}

	return thumbstick;
}


XMVECTOR Gamepad::NormalizeVector(XMVECTOR vector)
{
	XMVECTOR normalized;
	float magnitude = XMVector2Length(vector).m128_f32[0];

	if (magnitude < m_impl->m_deadzone)
	{
		normalized =  XMVectorSet(0, 0, 0, 0);
	}
	else
	{
		// Note: Vector has a magnitude above 1 on diagonals
		normalized = (vector / magnitude) * ((magnitude - m_impl->m_deadzone) / (1 - m_impl->m_deadzone));
	}

	return normalized;
}