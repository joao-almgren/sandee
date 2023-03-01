#pragma once
#include <d3d11.h>
#include "winrt/base.h"

class Graphics
{
public:
	Graphics() = default;
	Graphics(const Graphics& g) = delete;
	Graphics(Graphics&& g) = delete;
	Graphics& operator=(const Graphics& g) = delete;
	Graphics& operator=(Graphics&& g) = delete;
	~Graphics() = default;

	[[nodiscard]] bool initialize(HWND hWnd, int windowWidth, int windowHeight);
	void present() const;
	void resetRenderTarget() const;
	void clearScreen(const float (&color)[4] = { 0, 0, 0, 0 }) const;

	[[nodiscard]] winrt::com_ptr<ID3D11Device> getDevice() const { return pDevice; }
	[[nodiscard]] winrt::com_ptr<ID3D11DeviceContext> getDeviceContext() const { return pDeviceContext; }

protected:
	winrt::com_ptr<ID3D11Device> pDevice;
	winrt::com_ptr<ID3D11DeviceContext> pDeviceContext;
	winrt::com_ptr<IDXGISwapChain> pSwapChain;
};
