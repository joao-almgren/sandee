#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <winrt/base.h>

class Input
{
public:
	Input() = default;
	Input(const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator=(const Input&) = delete;
	Input& operator=(Input&&) = delete;
	~Input() = default;

	bool init(HWND hWnd, HINSTANCE hInstance);
	bool update();

	DIMOUSESTATE mouseState{};
	unsigned char keyState[256]{};

private:
	bool initMouse(HWND hWnd);
	bool initKeyboard(HWND hWnd);

	bool updateMouse();
	bool updateKeyboard();

	winrt::com_ptr<IDirectInput> m_pDevice{ nullptr };
	winrt::com_ptr<IDirectInputDevice> m_pMouse{ nullptr };
	winrt::com_ptr<IDirectInputDevice> m_pKeyboard{ nullptr };
};
