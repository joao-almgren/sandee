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

void Graphics::drawTest()
{
	struct Vertex
	{
		float x, y;
	} const vertices[]
	{
		{   0,  0.5},
		{ 0.5, -0.5},
		{-0.5, -0.5},
	};

	const D3D11_BUFFER_DESC bufferDesc
	{
		.ByteWidth{ sizeof(vertices) },
		.Usage{ D3D11_USAGE_DEFAULT },
		.BindFlags{ D3D11_BIND_VERTEX_BUFFER },
		.CPUAccessFlags{ 0 },
		.MiscFlags{ 0 },
		.StructureByteStride { sizeof (Vertex) },
	};

	const D3D11_SUBRESOURCE_DATA subresourceData
	{
		.pSysMem{ vertices }
	};

	winrt::com_ptr<ID3D11Buffer> pVertexBuffer;
	HRESULT hr = pDevice->CreateBuffer(&bufferDesc, &subresourceData, pVertexBuffer.put());
	if (FAILED(hr))
		return;

	ID3D11Buffer* pVertex{ pVertexBuffer.get() };
	const UINT stride = sizeof(Vertex);
	pDeviceContext->IASetVertexBuffers(0, 1, &pVertex, &stride, nullptr);

	winrt::com_ptr<ID3DBlob> pBlob;
	hr = D3DReadFileToBlob(L"VertexShader.cso", pBlob.put());
	winrt::com_ptr<ID3D11VertexShader> pVertexShader;
	hr = pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pVertexShader.put());
	pDeviceContext->VSSetShader(pVertexShader.get(), nullptr, 0);

	const D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	winrt::com_ptr<ID3D11InputLayout> pInputLayout;
	hr = pDevice->CreateInputLayout(inputElementDesc, std::size(inputElementDesc), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), pInputLayout.put());

	pBlob = nullptr;
	hr = D3DReadFileToBlob(L"PixelShader.cso", pBlob.put());
	winrt::com_ptr<ID3D11PixelShader> pPixelShader;
	hr = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, pPixelShader.put());
	pDeviceContext->PSSetShader(pPixelShader.get(), nullptr, 0);

	winrt::com_ptr<ID3D11Resource> pBackBuffer;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), pBackBuffer.put_void());
	winrt::com_ptr<ID3D11RenderTargetView> pRenderTarget;
	hr = pDevice->CreateRenderTargetView(pBackBuffer.get(), nullptr, pRenderTarget.put());

	ID3D11RenderTargetView* pTarget{ pRenderTarget.get() };
	pDeviceContext->OMSetRenderTargets(1, &pTarget, nullptr);

	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_VIEWPORT viewport
	{
		.TopLeftX{ 0 },
		.TopLeftY{ 0 },
		.Width{ 800 },
		.Height{ 600 },
		.MinDepth{ 0 },
		.MaxDepth{ 1 },
	};
	pDeviceContext->RSSetViewports(1, &viewport);

	pDeviceContext->Draw(std::size(vertices), 0);
}
