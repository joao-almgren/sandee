#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "graphics.h"
#include <d3dcompiler.h>
#include <cassert>

bool Graphics::initialize(const HWND hWnd)
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
		.BufferCount = 1,
		.OutputWindow = hWnd,
		.Windowed = TRUE,
		.SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
		.Flags = 0,
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
	const HRESULT hr = pSwapChain->Present(1, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED)
	{
		throw GraphicsDeviceRemovedException("DXGI_ERROR_DEVICE_REMOVED");
	}
	assert(SUCCEEDED(hr));
}

void Graphics::clearScreen(const float (&color)[4]) const
{
	winrt::com_ptr<ID3D11Resource> pBackBuffer;
	HRESULT hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), pBackBuffer.put_void());
	assert(SUCCEEDED(hr));
	winrt::com_ptr<ID3D11RenderTargetView> pRenderTarget;
	hr = pDevice->CreateRenderTargetView(pBackBuffer.get(), nullptr, pRenderTarget.put());
	assert(SUCCEEDED(hr));
	pDeviceContext->ClearRenderTargetView(pRenderTarget.get(), color);
}

void Graphics::drawTest()
{
	struct Vertex
	{
		float x, y;
	} const vertices[]
	{
		{  0.0f,  0.5f },
		{  0.5f, -0.5f },
		{ -0.5f, -0.5f },
	};

	const D3D11_BUFFER_DESC bufferDesc
	{
		.ByteWidth = sizeof(vertices),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = sizeof (Vertex),
	};

	const D3D11_SUBRESOURCE_DATA subresourceData
	{
		.pSysMem = vertices
	};

	winrt::com_ptr<ID3D11Buffer> pVertexBuffer;
	HRESULT hr = pDevice->CreateBuffer(&bufferDesc, &subresourceData, pVertexBuffer.put());
	assert(SUCCEEDED(hr));

	ID3D11Buffer* pVertex = pVertexBuffer.get();
	const UINT stride = sizeof(Vertex);
	const UINT offsets = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &pVertex, &stride, &offsets);

	winrt::com_ptr<ID3DBlob> pBlob;
	hr = D3DReadFileToBlob(L"VertexShader.cso", pBlob.put());
	assert(SUCCEEDED(hr));
	winrt::com_ptr<ID3D11VertexShader> pVertexShader;
	hr = pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pVertexShader.put());
	assert(SUCCEEDED(hr));
	pDeviceContext->VSSetShader(pVertexShader.get(), nullptr, 0);

	const D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	winrt::com_ptr<ID3D11InputLayout> pInputLayout;
	hr = pDevice->CreateInputLayout(inputElementDesc, static_cast<unsigned>(std::size(inputElementDesc)), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), pInputLayout.put());
	assert(SUCCEEDED(hr));
	pDeviceContext->IASetInputLayout(pInputLayout.get());

	pBlob = nullptr;
	hr = D3DReadFileToBlob(L"PixelShader.cso", pBlob.put());
	assert(SUCCEEDED(hr));
	winrt::com_ptr<ID3D11PixelShader> pPixelShader;
	hr = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pPixelShader.put());
	assert(SUCCEEDED(hr));
	pDeviceContext->PSSetShader(pPixelShader.get(), nullptr, 0);

	winrt::com_ptr<ID3D11Resource> pBackBuffer;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), pBackBuffer.put_void());
	assert(SUCCEEDED(hr));
	winrt::com_ptr<ID3D11RenderTargetView> pRenderTarget;
	hr = pDevice->CreateRenderTargetView(pBackBuffer.get(), nullptr, pRenderTarget.put());
	assert(SUCCEEDED(hr));

	ID3D11RenderTargetView* pTarget = pRenderTarget.get();
	pDeviceContext->OMSetRenderTargets(1, &pTarget, nullptr);

	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const D3D11_VIEWPORT viewport
	{
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = 800,
		.Height = 600,
		.MinDepth = 0,
		.MaxDepth = 1,
	};
	pDeviceContext->RSSetViewports(1, &viewport);

	pDeviceContext->Draw(static_cast<unsigned>(std::size(vertices)), 0);
}
