#include "graphics.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
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
		.CullMode = D3D11_CULL_BACK,
		.FrontCounterClockwise = TRUE,
		.DepthBias = 0,
		.DepthBiasClamp = 0.0f,
		.SlopeScaledDepthBias = 0.0f,
		.DepthClipEnable = TRUE,
		.ScissorEnable = FALSE,
		.MultisampleEnable = FALSE,
		.AntialiasedLineEnable = FALSE,
	};

	hr = m_pDevice->CreateRasterizerState(&rasterizerDesc, m_pDefaultRasterizerState.put());
	if (FAILED(hr))
		return false;

	m_pDeviceContext->RSSetState(m_pDefaultRasterizerState.get());

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

void Graphics::resetRasterizer() const
{
	m_pDeviceContext->RSSetState(m_pDefaultRasterizerState.get());
}

void Graphics::present() const
{
	m_pSwapChain->Present(1, 0);
}

void Graphics::resetRenderTarget() const
{
	ID3D11RenderTargetView * pTargets[]{ m_pRenderTarget.get() };
	m_pDeviceContext->OMSetRenderTargets(1, pTargets, m_pDepthStencilView.get());
}

void Graphics::clearScreen(const float (& color)[4]) const
{
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTarget.get(), color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView.get(), D3D11_CLEAR_DEPTH, 1, 0);
}

bool Graphics::loadDDSTexture(const wchar_t * filename, ID3D11ShaderResourceView ** pTextureView) const
{
	HRESULT hr = DirectX::CreateDDSTextureFromFile(m_pDevice.get(), filename, nullptr, pTextureView);
	if (FAILED(hr))
		return false;

	return true;
}

bool Graphics::loadWICTexture(const wchar_t* filename, ID3D11ShaderResourceView** pTextureView) const
{
	HRESULT hr = DirectX::CreateWICTextureFromFile(m_pDevice.get(), filename, nullptr, pTextureView);
	if (FAILED(hr))
		return false;

	return true;
}

bool Graphics::loadTexture(const char* const filename, ID3D11ShaderResourceView** pTextureView) const
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

	winrt::com_ptr<ID3D11Texture2D> pTexture{ nullptr };
	HRESULT hr = m_pDevice->CreateTexture2D(&textureDesc, &textureSubresourceData, pTexture.put());
	free(textureBytes);
	if (FAILED(hr))
		return false;

	hr = m_pDevice->CreateShaderResourceView(pTexture.get(), nullptr, pTextureView);
	if (FAILED(hr))
		return false;

	return true;
}

bool Graphics::loadVertexBuffer(const void* pVertices, const UINT numVertices, const UINT vertexSize, ID3D11Buffer** pBuffer) const
{
	const D3D11_BUFFER_DESC vertexBufferDesc
	{
		.ByteWidth = numVertices * vertexSize,
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = vertexSize,
	};

	const D3D11_SUBRESOURCE_DATA vertexSubresourceData
	{
		.pSysMem = pVertices,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hr = m_pDevice->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, pBuffer);
	if (FAILED(hr))
		return false;

	return true;
}

bool Graphics::loadIndexBuffer(const void* pIndices, const UINT numIndices, const UINT indexSize, ID3D11Buffer** pBuffer) const
{
	const D3D11_BUFFER_DESC indexBufferDesc
	{
		.ByteWidth = numIndices * indexSize,
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = indexSize,
	};

	const D3D11_SUBRESOURCE_DATA indexSubresourceData
	{
		.pSysMem = pIndices,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hr = m_pDevice->CreateBuffer(&indexBufferDesc, &indexSubresourceData, pBuffer);
	if (FAILED(hr))
		return false;

	return true;
}

bool Graphics::createConstantBuffer(const UINT constantSize, ID3D11Buffer** pBuffer) const
{
	const D3D11_BUFFER_DESC constantBufferDesc
	{
		.ByteWidth = constantSize,
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	HRESULT hr = m_pDevice->CreateBuffer(&constantBufferDesc, nullptr, pBuffer);
	if (FAILED(hr))
		return false;

	return true;
}

bool Graphics::loadVertexShader(const wchar_t* const filename, ID3D11VertexShader** pShader, const D3D11_INPUT_ELEMENT_DESC* pLayout, const UINT numElements, ID3D11InputLayout** pInput) const
{
	winrt::com_ptr<ID3DBlob> pBlob;
	HRESULT hr = D3DReadFileToBlob(filename, pBlob.put());
	if (FAILED(hr))
		return false;

	hr = m_pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pShader);
	if (FAILED(hr))
		return false;

	hr = m_pDevice->CreateInputLayout(pLayout, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), pInput);
	if (FAILED(hr))
		return false;

	return true;
}

bool Graphics::loadPixelShader(const wchar_t* const filename, ID3D11PixelShader** pBuffer) const
{
	winrt::com_ptr<ID3DBlob> pBlob;
	HRESULT hr = D3DReadFileToBlob(filename, pBlob.put());
	if (FAILED(hr))
		return false;

	hr = m_pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pBuffer);
	if (FAILED(hr))
		return false;

	return true;
}
