#include "Gamepad.h"
#include <math.h>
#include "WriteLog.h"


Gamepad::Gamepad()
{
	m_deadzone = 0.2f;
	m_controllerId = -1;
}


bool Gamepad::Update()
{
	// Note: overkill to check connection every frame?
	if (m_controllerId == -1)
	{
		if (!CheckConnection())
		{
			return false;
		}
	}

	ZeroMemory(&m_state, sizeof(XINPUT_STATE));
	if (XInputGetState(m_controllerId, &m_state) != ERROR_SUCCESS)
	{
		m_controllerId = -1;
		return false;
	}

	return true;
}


bool Gamepad::GetButtonState(GamepadButtons button)
{
	bool buttonState = false;

	switch (button)
	{
		case DPAD_UP:
			buttonState = (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
			break;
		case DPAD_DOWN:
			buttonState = (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
			break;
		case DPAD_LEFT:
			buttonState = (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
			break;
		case DPAD_RIGHT:
			buttonState = (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
			break;
		case START_BUTTON:
			buttonState = (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
			break;
		case BACK_BUTTON:
			buttonState = (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
			break;
		case LEFT_THUMB:
			buttonState = (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0;
			break;
		case RIGHT_THUMB:
			buttonState = (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0;
			break;
		case RIGHT_SHOULDER:
			buttonState = (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
			break;
		case LEFT_SHOULDER:
			buttonState = (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
			break;
		case A_BUTTON:
			buttonState = (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
			break;
		case B_BUTTON:
			buttonState = (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
			break;
		case X_BUTTON:
			buttonState = (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
			break;
		case Y_BUTTON:
			buttonState = (m_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
			break;
		default: 
			DEBUG_WARN("Gamepad button not recognized");
			break;
			
	}

	return buttonState;
}


float Gamepad::GetAxisState(GamepadAxes axis)
{
	float axisState = 0;

	switch (axis)
	{
		case LEFT_THUMB_X:
			axisState = NormalizeThumbstick(m_state.Gamepad.sThumbLX);
			break;
		case LEFT_THUMB_Y:
			axisState = NormalizeThumbstick(m_state.Gamepad.sThumbLY);
			break;
		case RIGHT_THUMB_X:
			axisState = NormalizeThumbstick(m_state.Gamepad.sThumbRX);
			break;
		case RIGHT_THUMB_Y:
			axisState = NormalizeThumbstick(m_state.Gamepad.sThumbRY);
			break;
		case LEFT_TRIGGER: 
			axisState = (float)m_state.Gamepad.bLeftTrigger / 255;
			break;
		case RIGHT_TRIGGER: 
			axisState = (float)m_state.Gamepad.bRightTrigger / 255;
			break;
		default:
			DEBUG_WARN("Gamepad axis not recognized");
			break;
	}

	return axisState;
}


XMVECTOR Gamepad::GetRightThumbstickVector()
{
	float normX = fmaxf(-1, (float)m_state.Gamepad.sThumbRX / 32767);
	float normY = fmaxf(-1, (float)m_state.Gamepad.sThumbRY / 32767);
	XMVECTOR vector = XMVectorSet(normX, normY, 0, 0);

	return NormalizeVector(vector);
}


XMVECTOR Gamepad::GetLeftThumbstickVector()
{
	float normX = fmaxf(-1, (float)m_state.Gamepad.sThumbLX / 32767);
	float normY = fmaxf(-1, (float)m_state.Gamepad.sThumbLY / 32767);
	XMVECTOR vector = XMVectorSet(normX, normY, 0, 0);

	return NormalizeVector(vector);
}


void Gamepad::SetDeadzone(float deadzone)
{
	m_deadzone = deadzone;
}


float Gamepad::GetDeadzone() const
{
	return m_deadzone;
}


int Gamepad::GetPort()
{
	return m_controllerId + 1;
}


bool Gamepad::IsConnected()
{
	return m_controllerId != -1;
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

	m_controllerId = controllerId;

	return controllerId != -1;
}


float Gamepad::NormalizeThumbstick(int axis)
{
	float normal = fmaxf(-1, (float)axis / 32767);
	float thumbstick = (fabs(normal) < m_deadzone ? 0 : (fabs(normal) - m_deadzone) * (normal / fabs(normal)));
	if (m_deadzone > 0)
	{
		thumbstick /= 1 - m_deadzone;
	}

	return thumbstick;
}


XMVECTOR Gamepad::NormalizeVector(XMVECTOR vector)
{
	XMVECTOR normalized;
	float magnitude = XMVector2Length(vector).m128_f32[0];

	if (magnitude < m_deadzone)
	{
		normalized =  XMVectorSet(0, 0, 0, 0);
	}
	else
	{
		// Note: Vector has a magnitude above 1 on diagonals
		normalized = XMVector2Normalize(vector) * ((magnitude - m_deadzone) / (1 - m_deadzone));
	}

	return normalized;
}