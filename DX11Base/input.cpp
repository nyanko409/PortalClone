#include "input.h"
#include "main.h"


IDirectInput8* CInput::m_directInput;
IDirectInputDevice8* CInput::m_keyboard;
IDirectInputDevice8* CInput::m_mouse;

unsigned char CInput::m_keyboardState[256];
unsigned char CInput::m_oldKeyboardState[256];
DIMOUSESTATE CInput::m_mouseState;
BYTE CInput::m_oldMouseState[4];


void CInput::Init()
{
	// create the direct input interface
	DirectInput8Create(GetInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);

	// init keyboard
	m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	m_keyboard->SetCooperativeLevel(GetWindow(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	m_keyboard->Acquire();

	// init mouse
	m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	m_mouse->SetDataFormat(&c_dfDIMouse);
	m_mouse->SetCooperativeLevel(GetWindow(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_mouse->Acquire();
}

void CInput::Uninit()
{
	// Release the mouse.
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Release the keyboard.
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Release the main interface to direct input.
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}
}

void CInput::Update()
{
	ReadKeyboard();
	ReadMouse();
}

bool CInput::ReadKeyboard()
{
	HRESULT result;

	// Read the keyboard device.
	memcpy(m_oldKeyboardState, m_keyboardState, 256);
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);

	if (FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			m_keyboard->Acquire();
		else
			return false;
	}

	return true;
}

bool CInput::ReadMouse()
{
	HRESULT result;
	
	// Read the mouse device.
	memcpy(m_oldMouseState, m_mouseState.rgbButtons, 4);
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);

	if (FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			m_mouse->Acquire();
		else
			return false;
	}

	return true;
}

bool CInput::GetKeyPress(BYTE dikCode)
{
	return m_keyboardState[dikCode] & 0x80;
}

bool CInput::GetKeyTrigger(BYTE dikCode)
{
	return (m_keyboardState[dikCode] & 0x80) && 
		!(m_oldKeyboardState[dikCode] & 0x80);
}

bool CInput::GetMouseLeftTrigger()
{
	return m_mouseState.rgbButtons[0] && !m_oldMouseState[0];
}

bool CInput::GetMouseRightTrigger()
{
	return m_mouseState.rgbButtons[1] && !m_oldMouseState[1];
}

bool CInput::GetMouseMiddleTrigger()
{
	return m_mouseState.rgbButtons[2] && !m_oldMouseState[2];
}
