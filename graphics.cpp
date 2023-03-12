#include "graphics.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.Flags = 0,
	};

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		m_pSwapChain.put(),
		m_pDevice.put(),
		nullptr,
		m_pDeviceContext.put()
	);

	if (FAILED(hr))
		return false;

	D3D11_RASTERIZER_DESC rasterizerDesc =
	{
		.FillMode = D3D11_FILL_SOLID,
		.CullMode = D3D11_CULL_BACK,
		.FrontCounterClockwise = FALSE,
		.DepthBias = 0,
		.DepthBiasClamp = 0.0f,
		.SlopeScaledDepthBias = 0.0f,
		.DepthClipEnable = TRUE,
		.ScissorEnable = FALSE,
		.MultisampleEnable = FALSE,
		.AntialiasedLineEnable = FALSE,
	};

	ID3D11RasterizerState * pRasterizerState;
	hr = m_pDevice->CreateRasterizerState(&rasterizerDesc, &pRasterizerState);
	if (FAILED(hr))
		return false;

	m_pDeviceContext->RSSetState(pRasterizerState);
	pRasterizerState->Release();

	winrt::com_ptr<ID3D11Resource> pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), pBackBuffer.put_void());
	if (FAILED(hr))
		return false;

	hr = m_pDevice->CreateRenderTargetView(pBackBuffer.get(), nullptr, m_pRenderTarget.put());
	if (FAILED(hr))
		return false;

	D3D11_TEXTURE2D_DESC descDepth =
	{
		descDepth.Width = windowWidth,
		descDepth.Height = windowWidth,
		descDepth.MipLevels = 1,
		descDepth.ArraySize = 1,
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
		descDepth.SampleDesc.Count = 1,
		descDepth.SampleDesc.Quality = 0,
		descDepth.Usage = D3D11_USAGE_DEFAULT,
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL,
		descDepth.CPUAccessFlags = 0,
		descDepth.MiscFlags = 0,
	};

	hr = m_pDevice->CreateTexture2D(&descDepth, nullptr, m_pDepthStencil.put());
	if (FAILED(hr))
		return false;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV =
	{
		.Format = descDepth.Format,
		.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
		.Texture2D =
		{
			.MipSlice = 0,
		},
	};

	hr = m_pDevice->CreateDepthStencilView(m_pDepthStencil.get(), &descDSV, m_pDepthStencilView.put());
	if (FAILED(hr))
		return false;

	const D3D11_VIEWPORT viewport[] {
	{
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = static_cast<float>(windowWidth),
		.Height = static_cast<float>(windowHeight),
		.MinDepth = 0,
		.MaxDepth = 1,
	}};

	m_pDeviceContext->RSSetViewports(1, viewport);

	return true;
}

void Graphics::present() const
{
	m_pSwapChain->Present(1, 0);
}

void Graphics::resetRenderTarget() const
{
	ID3D11RenderTargetView * pTargets[]{ m_pRenderTarget.get() };
	m_pDeviceContext->OMSetRenderTargets(1, pTargets, nullptr);
}

void Graphics::clearScreen(const float (& color)[4]) const
{
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTarget.get(), color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView.get(), D3D11_CLEAR_DEPTH, 1, 0);
}

bool Graphics::loadTexture(const char * const filename, ID3D11Texture2D ** pTexture, ID3D11ShaderResourceView ** pTextureView) const
{
	int texWidth, texHeight, texNumChannels;
	unsigned char * textureBytes = stbi_load(filename, &texWidth, &texHeight, &texNumChannels, 4);
	if (!textureBytes)
		return false;

	D3D11_TEXTURE2D_DESC textureDesc =
	{
		.Width = static_cast<unsigned>(texWidth),
		.Height = static_cast<unsigned>(texHeight),
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.SampleDesc =
		{
			.Count = 1,
		},
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_SHADER_RESOURCE,
	};

	D3D11_SUBRESOURCE_DATA textureSubresourceData =
	{
		.pSysMem = textureBytes,
		.SysMemPitch = 4 * static_cast<unsigned>(texWidth),
	};

	HRESULT hr = m_pDevice->CreateTexture2D(&textureDesc, &textureSubresourceData, pTexture);
	free(textureBytes);
	if (FAILED(hr))
		return false;

	hr = m_pDevice->CreateShaderResourceView(*pTexture, nullptr, pTextureView);
	if (FAILED(hr))
	{
		(*pTexture)->Release();
		return false;
	}

	return true;
}
