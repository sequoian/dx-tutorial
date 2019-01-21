#pragma once

#include <Windows.h>
#include <Xinput.h>
#include <DirectXMath.h>
using namespace DirectX;

enum GamepadButtons
{
	DPAD_UP,
	DPAD_DOWN,
	DPAD_LEFT,
	DPAD_RIGHT,
	START_BUTTON,
	BACK_BUTTON,
	LEFT_THUMB,
	RIGHT_THUMB,
	RIGHT_SHOULDER,
	LEFT_SHOULDER,
	A_BUTTON,
	B_BUTTON,
	X_BUTTON,
	Y_BUTTON
};


enum GamepadAxes
{
	LEFT_THUMB_X,
	LEFT_THUMB_Y,
	RIGHT_THUMB_X,
	RIGHT_THUMB_Y,
	LEFT_TRIGGER,
	RIGHT_TRIGGER
};


class Gamepad
{
public:
	Gamepad();

	// called every frame
	bool Update();

	bool GetButtonState(GamepadButtons button);
	float GetAxisState(GamepadAxes axis);
	XMVECTOR GetRightThumbstickVector();
	XMVECTOR GetLeftThumbstickVector();

	void SetDeadzone(float deadzone);
	float GetDeadzone() const;
	int GetPort();
	bool IsConnected();

private:
	bool CheckConnection();
	float NormalizeThumbstick(int axis);
	XMVECTOR NormalizeVector(XMVECTOR vector);

private:
	XINPUT_STATE m_state;
	int m_controllerId;
	float m_deadzone;
};