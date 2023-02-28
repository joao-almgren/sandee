#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include "winrt/base.h"

class GraphicsDeviceRemovedException
{
public:
	GraphicsDeviceRemovedException() = default;
};

class Graphics
{
public:
	Graphics() = default;
	Graphics(const Graphics& g) = delete;
	Graphics(Graphics&& g) = delete;
	Graphics& operator=(const Graphics& g) = delete;
	Graphics& operator=(Graphics&& g) = delete;
	~Graphics() = default;

	[[nodiscard]] bool initialize(HWND hWnd);
	void present() const;

	void clearScreen(const float (&color)[4] = {}) const;
	void drawTest();

protected:
	winrt::com_ptr<ID3D11Device> pDevice;
	winrt::com_ptr<ID3D11DeviceContext> pDeviceContext;
	winrt::com_ptr<IDXGISwapChain> pSwapChain;
};
