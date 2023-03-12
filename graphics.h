#pragma once
#include <d3d11.h>
#include <winrt/base.h>

class Graphics
{
public:
	Graphics() = default;
	Graphics(const Graphics&) = default;
	Graphics(Graphics&&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	Graphics& operator=(Graphics&&) = delete;
	~Graphics() = default;

	[[nodiscard]] bool initialize(HWND hWnd, int windowWidth, int windowHeight);
	void present() const;
	void resetRenderTarget() const;
	void clearScreen(const float (& color)[4] = { 0, 0, 0, 0 }) const;

	[[nodiscard]] winrt::com_ptr<ID3D11Device> getDevice() const { return m_pDevice; }
	[[nodiscard]] winrt::com_ptr<ID3D11DeviceContext> getDeviceContext() const { return m_pDeviceContext; }

	bool loadTexture(const char * filename, ID3D11Texture2D ** pTexture, ID3D11ShaderResourceView ** pTextureView) const;

protected:
	winrt::com_ptr<ID3D11Device> m_pDevice{ nullptr };
	winrt::com_ptr<ID3D11DeviceContext> m_pDeviceContext{ nullptr };
	winrt::com_ptr<IDXGISwapChain> m_pSwapChain{ nullptr };
	winrt::com_ptr<ID3D11RenderTargetView> m_pRenderTarget{ nullptr };
	winrt::com_ptr<ID3D11Texture2D> m_pDepthStencil{ nullptr };
	winrt::com_ptr<ID3D11DepthStencilView> m_pDepthStencilView{ nullptr };
};
