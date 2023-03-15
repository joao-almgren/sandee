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

	const D3D11_RASTERIZER_DESC rasterizerDesc
	{
		.FillMode = D3D11_FILL_SOLID,
		.CullMode = D3D11_CULL_NONE,
		.FrontCounterClockwise = TRUE,
		.DepthBias = 0,
		.DepthBiasClamp = 0.0f,
		.SlopeScaledDepthBias = 0.0f,
		.DepthClipEnable = TRUE,
		.ScissorEnable = FALSE,
		.MultisampleEnable = FALSE,
		.AntialiasedLineEnable = FALSE,
	};

	winrt::com_ptr<ID3D11RasterizerState> pRasterizerState;
	hr = m_pDevice->CreateRasterizerState(&rasterizerDesc, pRasterizerState.put());
	if (FAILED(hr))
		return false;

	m_pDeviceContext->RSSetState(pRasterizerState.get());

	winrt::com_ptr<ID3D11Resource> pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), pBackBuffer.put_void());
	if (FAILED(hr))
		return false;

	hr = m_pDevice->CreateRenderTargetView(pBackBuffer.get(), nullptr, m_pRenderTarget.put());
	if (FAILED(hr))
		return false;

	const D3D11_TEXTURE2D_DESC depthDesc
	{
		.Width = static_cast<unsigned>(windowWidth),
		.Height = static_cast<unsigned>(windowWidth),
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_D32_FLOAT,
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0,
		},
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_DEPTH_STENCIL,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
	};

	hr = m_pDevice->CreateTexture2D(&depthDesc, nullptr, m_pDepthStencil.put());
	if (FAILED(hr))
		return false;

	const D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc
	{
		.Format = depthDesc.Format,
		.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
		.Flags = 0,
		.Texture2D
		{
			.MipSlice = 0,
		},
	};

	hr = m_pDevice->CreateDepthStencilView(m_pDepthStencil.get(), &depthStencilViewDesc, m_pDepthStencilView.put());
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
	if (textureBytes == nullptr)
		return false;

	const D3D11_TEXTURE2D_DESC textureDesc
	{
		.Width = static_cast<unsigned>(texWidth),
		.Height = static_cast<unsigned>(texHeight),
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.SampleDesc
		{
			.Count = 1,
		},
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_SHADER_RESOURCE,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
	};

	const D3D11_SUBRESOURCE_DATA textureSubresourceData
	{
		.pSysMem = textureBytes,
		.SysMemPitch = 4 * static_cast<unsigned>(texWidth),
		.SysMemSlicePitch = 0,
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
