#include "Keyboard.h"

#include <dinput.h>
#include "WriteLog.h"
#include "Assert.h"
#include "SampleWindow.h"


struct Keyboard::Impl
{
	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	unsigned char keyboardState[256];
	bool acquired;
};


Keyboard::Keyboard()
{
	m_impl = nullptr;
}


Keyboard::~Keyboard()
{
	ShutDown();
}


bool Keyboard::StartUp(SampleWindow& window)
{
	ASSERT(m_impl == nullptr);
	m_impl = new Impl;
	m_impl->directInput = nullptr;
	m_impl->keyboard = nullptr;
	m_impl->acquired = false;

	HRESULT hr;
	hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_impl->directInput, NULL);
	if (hr != S_OK)
	{
		DEBUG_ERROR("Failed to create Direct Input");
		return false;
	}

	hr = m_impl->directInput->CreateDevice(GUID_SysKeyboard, &m_impl->keyboard, NULL);
	if (hr != S_OK)
	{
		DEBUG_ERROR("Failed to create keyboard device");
		return false;
	}

	hr = m_impl->keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (hr != S_OK)
	{
		DEBUG_ERROR("Failed to set data format for keyboard");
		return false;
	}

	hr = m_impl->keyboard->SetCooperativeLevel(window.Window(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (hr != S_OK)
	{
		DEBUG_ERROR("Failed to set cooperative level for keyboard");
		return false;
	}

	return true;
}


void Keyboard::ShutDown()
{
	if (m_impl)
	{
		if (m_impl->keyboard)
		{
			m_impl->keyboard->Unacquire();
			m_impl->keyboard->Release();
			m_impl->keyboard = nullptr;
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


bool Keyboard::Update()
{
	if (!m_impl->acquired)
	{
		if (!AcquireKeyboard())
		{
			return false;
		}
	}

	HRESULT hr = m_impl->keyboard->GetDeviceState(sizeof(m_impl->keyboardState), (LPVOID)&m_impl->keyboardState);
	if (hr != S_OK)
	{
		m_impl->acquired = false;
		return false;
	}

	return true;
}


bool Keyboard::GetKeyState(KeyboardKeys key)
{
	// static lookup table for DirectInput (must match Key enum order)
	static const unsigned int s_keys[] = {
		DIK_ESCAPE,
		DIK_1,
		DIK_2,
		DIK_3,
		DIK_4,
		DIK_5,
		DIK_6,
		DIK_7,
		DIK_8,
		DIK_9,
		DIK_0,
		DIK_MINUS,
		DIK_EQUALS,
		DIK_BACK,
		DIK_TAB,
		DIK_Q,
		DIK_W,
		DIK_E,
		DIK_R,
		DIK_T,
		DIK_Y,
		DIK_U,
		DIK_I,
		DIK_O,
		DIK_P,
		DIK_LBRACKET,
		DIK_RBRACKET,
		DIK_RETURN,
		DIK_LCONTROL,
		DIK_A,
		DIK_S,
		DIK_D,
		DIK_F,
		DIK_G,
		DIK_H,
		DIK_J,
		DIK_K,
		DIK_L,
		DIK_SEMICOLON,
		DIK_APOSTROPHE,
		DIK_GRAVE,
		DIK_LSHIFT,
		DIK_BACKSLASH,
		DIK_Z,
		DIK_X,
		DIK_C,
		DIK_V,
		DIK_B,
		DIK_N,
		DIK_M,
		DIK_COMMA,
		DIK_PERIOD,
		DIK_SLASH,
		DIK_RSHIFT,
		DIK_MULTIPLY,
		DIK_LMENU,
		DIK_SPACE,
		DIK_CAPITAL,
		DIK_F1,
		DIK_F2,
		DIK_F3,
		DIK_F4,
		DIK_F5,
		DIK_F6,
		DIK_F7,
		DIK_F8,
		DIK_F9,
		DIK_F10,
		DIK_NUMLOCK,
		DIK_SCROLL,
		DIK_NUMPAD7,
		DIK_NUMPAD8,
		DIK_NUMPAD9,
		DIK_SUBTRACT,
		DIK_NUMPAD4,
		DIK_NUMPAD5,
		DIK_NUMPAD6,
		DIK_ADD,
		DIK_NUMPAD1,
		DIK_NUMPAD2,
		DIK_NUMPAD3,
		DIK_NUMPAD0,
		DIK_DECIMAL,
		DIK_OEM_102,
		DIK_F11,
		DIK_F12,
		DIK_F13,
		DIK_F14,
		DIK_F15,
		DIK_KANA,
		DIK_ABNT_C1,
		DIK_CONVERT,
		DIK_NOCONVERT,
		DIK_YEN,
		DIK_ABNT_C2,
		DIK_NUMPADEQUALS,
		DIK_PREVTRACK,
		DIK_AT,
		DIK_COLON,
		DIK_UNDERLINE,
		DIK_KANJI,
		DIK_STOP,
		DIK_AX,
		DIK_UNLABELED,
		DIK_NEXTTRACK,
		DIK_NUMPADENTER,
		DIK_RCONTROL,
		DIK_MUTE,
		DIK_CALCULATOR,
		DIK_PLAYPAUSE,
		DIK_MEDIASTOP,
		DIK_VOLUMEDOWN,
		DIK_VOLUMEUP,
		DIK_WEBHOME,
		DIK_NUMPADCOMMA,
		DIK_DIVIDE,
		DIK_SYSRQ,
		DIK_RMENU,
		DIK_PAUSE,
		DIK_HOME,
		DIK_UP,
		DIK_PRIOR,
		DIK_LEFT,
		DIK_RIGHT,
		DIK_END,
		DIK_DOWN,
		DIK_NEXT,
		DIK_INSERT,
		DIK_DELETE,
		DIK_LWIN,
		DIK_RWIN,
		DIK_APPS,
		DIK_POWER,
		DIK_SLEEP,
		DIK_WAKE,
		DIK_WEBSEARCH,
		DIK_WEBFAVORITES,
		DIK_WEBREFRESH,
		DIK_WEBSTOP,
		DIK_WEBFORWARD,
		DIK_WEBBACK,
		DIK_MYCOMPUTER,
		DIK_MAIL,
		DIK_MEDIASELECT
	};
	static const unsigned int s_numKeys = sizeof(s_keys) / sizeof(s_keys[0]);

	ASSERT_VERBOSE(key < s_numKeys, "Keyboard key not recognized");

	return (m_impl->keyboardState[s_keys[key]] & 0x80) != 0;
}


bool Keyboard::AcquireKeyboard()
{
	HRESULT hr = m_impl->keyboard->Acquire();
	if (hr != S_OK)
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			DEBUG_ERROR("Failed to acquire keyboard");
		}
		
		m_impl->acquired = false;
		return false;
	}

	m_impl->acquired = true;
	return true;
}