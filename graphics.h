#pragma once
#include <d3d11.h>

class Graphics
{
public:
	Graphics() = default;
	Graphics(Graphics& g) = delete;
	Graphics(Graphics&& g) = delete;
	Graphics& operator=(const Graphics& g) = delete;
	Graphics& operator=(Graphics&& g) = delete;
	~Graphics();

	[[nodiscard]] bool initialize(HWND hWnd);
	void present() const;

	void clearScreen(const float (&color)[4] = {}) const;

protected:
	ID3D11Device* pDevice{ nullptr };
	ID3D11DeviceContext* pDeviceContext{ nullptr };
	IDXGISwapChain* pSwapChain{ nullptr };
};
