#include "graphics.h"

Graphics::~Graphics()
{
	if (pDeviceContext)
	{
		pDeviceContext->Release();
	}

	if (pSwapChain)
	{
		pSwapChain->Release();
	}

	if (pDevice)
	{
		pDevice->Release();
	}
}

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
		&pSwapChain,
		&pDevice,
		nullptr,
		&pDeviceContext
	)))
	{
		return false;
	}

	return true;
}

void Graphics::present() const
{
	pSwapChain->Present(1, 0);
}

void Graphics::clearScreen(const float (&color)[4]) const
{
	ID3D11Resource* pBuffer{ nullptr };
	if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBuffer))))
	{
		ID3D11RenderTargetView* pTarget{ nullptr };
		if (SUCCEEDED(pDevice->CreateRenderTargetView(pBuffer, nullptr, &pTarget)))
		{
			pDeviceContext->ClearRenderTargetView(pTarget, color);
			pTarget->Release();
		}
		pBuffer->Release();
	}
}
