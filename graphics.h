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
	void cls(const float colour[4]) const;

	[[nodiscard]] IDXGISwapChain* swap() const;

protected:
	ID3D11Device* pDevice{ nullptr };
	ID3D11DeviceContext* pContext{ nullptr };
	IDXGISwapChain* pSwap{ nullptr };
};
