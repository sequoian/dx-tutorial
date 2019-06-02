#pragma once

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
	LEFT_SHOULDER,
	RIGHT_SHOULDER,
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
	~Gamepad();
	bool StartUp();
	void ShutDown();

	// called every frame
	bool Update();

	bool GetButtonState(GamepadButtons button) const;
	float GetAxisState(GamepadAxes axis) const;
	XMVECTOR GetRightThumbstickVector();
	XMVECTOR GetLeftThumbstickVector();

	void SetDeadzone(float deadzone);
	float GetDeadzone() const;
	int GetPort();
	bool IsConnected();

private:
	bool CheckConnection();
	float NormalizeThumbstick(int axis) const;
	XMVECTOR NormalizeVector(XMVECTOR vector);

private:
	struct Impl;
	Impl* m_impl = nullptr;
};