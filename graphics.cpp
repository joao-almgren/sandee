#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "graphics.h"

bool Graphics::initialize(const HWND hWnd, const int windowWidth, const int windowHeight)
{
	const DXGI_SWAP_CHAIN_DESC swapChainDesc
	{
		.BufferDesc
		{
			.Width = 0,
			.Height = 0,
			.RefreshRate
			{
				.Numerator = 0,
				.Denominator = 0,
			},
			.Format = DXGI_FORMAT_B8G8R8A8_UNORM,
			.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
			.Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
		},
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 2,
		.OutputWindow = hWnd,
		.Windowed = TRUE,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
		.Flags = 0,
	};

	if (FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		pSwapChain.put(),
		pDevice.put(),
		nullptr,
		pDeviceContext.put()
	)))
	{
		return false;
	}

	const D3D11_VIEWPORT viewport[] {
	{
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = static_cast<float>(windowWidth),
		.Height = static_cast<float>(windowHeight),
		.MinDepth = 0,
		.MaxDepth = 1,
	}};

	pDeviceContext->RSSetViewports(1, viewport);

	return true;
}

void Graphics::present() const
{
	pSwapChain->Present(1, 0);
}

void Graphics::resetRenderTarget() const
{
	winrt::com_ptr<ID3D11Resource> pBackBuffer;
	if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), pBackBuffer.put_void())))
	{
		winrt::com_ptr<ID3D11RenderTargetView> pRenderTarget;
		if (SUCCEEDED(pDevice->CreateRenderTargetView(pBackBuffer.get(), nullptr, pRenderTarget.put())))
		{
			ID3D11RenderTargetView* pTarget[]{ pRenderTarget.get() };
			pDeviceContext->OMSetRenderTargets(1, pTarget, nullptr);
		}
	}
}

void Graphics::clearScreen(const float (&color)[4]) const
{
	winrt::com_ptr<ID3D11Resource> pBackBuffer;
	if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), pBackBuffer.put_void())))
	{
		winrt::com_ptr<ID3D11RenderTargetView> pRenderTarget;
		if (SUCCEEDED(pDevice->CreateRenderTargetView(pBackBuffer.get(), nullptr, pRenderTarget.put())))
		{
			pDeviceContext->ClearRenderTargetView(pRenderTarget.get(), color);
		}
	}
}
