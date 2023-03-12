#include "input.h"
#pragma comment(lib, "DXGuid.lib")
#pragma comment(lib, "DInput8.lib")

bool Input::init(const HWND hWnd, const HINSTANCE hInstance)
{
	if (DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8W, m_pDevice.put_void(), nullptr) != DI_OK)
		return false;

	return (initMouse(hWnd) && initKeyboard(hWnd));
}

bool Input::update()
{
	return (updateMouse() && updateKeyboard());
}

bool Input::initMouse(const HWND hWnd)
{
	if (m_pDevice->CreateDevice(GUID_SysMouse, m_pMouse.put(), nullptr) != DI_OK)
		return false;

	if (m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
		return false;

	if (m_pMouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
		return false;

	HRESULT hr = m_pMouse->Acquire();
	if (hr != DI_OK && hr != S_FALSE)
		return false;

	return true;
}

bool Input::updateMouse()
{
	if (m_pMouse->GetDeviceState(sizeof DIMOUSESTATE, &mouseState) != DI_OK)
		return false;

	return true;
}

bool Input::initKeyboard(const HWND hWnd)
{
	if (m_pDevice->CreateDevice(GUID_SysKeyboard, m_pKeyboard.put(), nullptr) != DI_OK)
		return false;

	if (m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
		return false;

	if (m_pKeyboard->SetDataFormat(&c_dfDIKeyboard) != DI_OK)
		return false;

	const HRESULT hr = m_pKeyboard->Acquire();
	if (hr != DI_OK && hr != S_FALSE)
		return false;

	return true;
}

bool Input::updateKeyboard()
{
	if (m_pKeyboard->GetDeviceState(256, &keyState) != DI_OK)
		return false;

	return true;
}
