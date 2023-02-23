#include "graphics.h"

Graphics::~Graphics()
{
	if (pContext)
	{
		pContext->Release();
	}

	if (pSwap)
	{
		pSwap->Release();
	}

	if (pDevice)
	{
		pDevice->Release();
	}
}

bool Graphics::initialize(const HWND hWnd)
{
	const DXGI_SWAP_CHAIN_DESC sd
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
		.BufferCount = 1,
		.OutputWindow = hWnd,
		.Windowed = TRUE,
		.SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
		.Flags = 0,
	};

	const HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Graphics::cls(const float colour[4]) const
{
	ID3D11Resource* pBuffer{ nullptr };
	if (SUCCEEDED(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBuffer))))
	{
		ID3D11RenderTargetView* pTarget{ nullptr };
		if (SUCCEEDED(pDevice->CreateRenderTargetView(pBuffer, nullptr, &pTarget)))
		{
			pContext->ClearRenderTargetView(pTarget, colour);
			pTarget->Release();
		}
		pBuffer->Release();
	}
}

[[nodiscard]] IDXGISwapChain* Graphics::swap() const
{
	return pSwap;
}
