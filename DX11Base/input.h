#pragma once

#include <dinput.h>

#define DIRECTINPUT_VERSION 0x0800


class CInput
{
private:
	static IDirectInput8* m_directInput;
	static IDirectInputDevice8* m_keyboard;
	static IDirectInputDevice8* m_mouse;
	 
	static unsigned char m_keyboardState[256];
	static unsigned char m_oldKeyboardState[256];
	static DIMOUSESTATE m_mouseState;
	static BYTE m_oldMouseState[4];

	static bool ReadKeyboard();
	static bool ReadMouse();

public:
	static void Init();
	static void Uninit();
	static void Update();

	static bool GetKeyPress( BYTE dikCode );
	static bool GetKeyTrigger( BYTE dikCode );

	static bool GetMouseLeftTrigger();
	static bool GetMouseRightTrigger();
	static bool GetMouseMiddleTrigger();
};
