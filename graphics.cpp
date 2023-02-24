#include "graphics.h"

bool Graphics::initialize(const HWND hWnd)
{
	const DXGI_SWAP_CHAIN_DESC swapChainDesc
	{
		.BufferDesc
		{
			.Width{ 0 },
			.Height{ 0 },
			.RefreshRate
			{
				.Numerator{ 0 },
				.Denominator{ 0 },
			},
			.Format{ DXGI_FORMAT_B8G8R8A8_UNORM },
			.ScanlineOrdering{ DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED },
			.Scaling{ DXGI_MODE_SCALING_UNSPECIFIED },
		},
		.SampleDesc
		{
			.Count{ 1 },
			.Quality{ 0 },
		},
		.BufferUsage{ DXGI_USAGE_RENDER_TARGET_OUTPUT },
		.BufferCount{ 1 },
		.OutputWindow{ hWnd },
		.Windowed{ TRUE },
		.SwapEffect{ DXGI_SWAP_EFFECT_DISCARD },
		.Flags{ 0 },
	};

	if (FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
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

	return true;
}

void Graphics::present() const
{
	HRESULT hr;
	if (FAILED(hr = pSwapChain->Present(1, 0)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GraphicsDeviceRemovedException();
		}
	}
}

void Graphics::clearScreen(const float (&color)[4]) const
{
	winrt::com_ptr<ID3D11Resource> pBuffer;
	if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), pBuffer.put_void())))
	{
		winrt::com_ptr<ID3D11RenderTargetView> pTarget;
		if (SUCCEEDED(pDevice->CreateRenderTargetView(pBuffer.get(), nullptr, pTarget.put())))
		{
			pDeviceContext->ClearRenderTargetView(pTarget.get(), color);
		}
	}
}
