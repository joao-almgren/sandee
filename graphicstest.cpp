#include "GraphicsTest.h"
#include <d3dcompiler.h>

namespace
{
	const D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	struct Vertex
	{
		float x, y;
	};
	
	const Vertex vertices[]
	{
		{  0.0f,  0.5f },
		{  0.5f, -0.5f },
		{ -0.5f, -0.5f },
	};
}

bool GraphicsTest::load(winrt::com_ptr<ID3D11Device> pDevice)
{
	const D3D11_BUFFER_DESC bufferDesc
	{
		.ByteWidth = sizeof(vertices),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = sizeof(Vertex),
	};

	const D3D11_SUBRESOURCE_DATA subresourceData
	{
		.pSysMem = vertices
	};

	HRESULT hr = pDevice->CreateBuffer(&bufferDesc, &subresourceData, pVertexBuffer.put());
	if (FAILED(hr))
		return false;

	winrt::com_ptr<ID3DBlob> pVertexBlob;
	hr = D3DReadFileToBlob(L"vertextest.cso", pVertexBlob.put());
	if (FAILED(hr))
		return false;

	hr = pDevice->CreateVertexShader(pVertexBlob->GetBufferPointer(), pVertexBlob->GetBufferSize(), nullptr, pVertexShader.put());
	if (FAILED(hr))
		return false;

	hr = pDevice->CreateInputLayout(inputElementDesc, static_cast<unsigned>(std::size(inputElementDesc)), pVertexBlob->GetBufferPointer(), pVertexBlob->GetBufferSize(), pInputLayout.put());
	if (FAILED(hr))
		return false;

	winrt::com_ptr<ID3DBlob> pPixelBlob;
	hr = D3DReadFileToBlob(L"pixeltest.cso", pPixelBlob.put());
	if (FAILED(hr))
		return false;

	hr = pDevice->CreatePixelShader(pPixelBlob->GetBufferPointer(), pPixelBlob->GetBufferSize(), nullptr, pPixelShader.put());
	if (FAILED(hr))
		return false;

	return true;
}

void GraphicsTest::draw(winrt::com_ptr<ID3D11DeviceContext> pDeviceContext) const
{
	const UINT offsets = 0;
	const UINT stride = sizeof(Vertex);
	ID3D11Buffer* pVertex[]{ pVertexBuffer.get() };
	pDeviceContext->IASetVertexBuffers(0, 1, pVertex, &stride, &offsets);
	pDeviceContext->VSSetShader(pVertexShader.get(), nullptr, 0);
	pDeviceContext->IASetInputLayout(pInputLayout.get());
	pDeviceContext->PSSetShader(pPixelShader.get(), nullptr, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->Draw(static_cast<unsigned>(std::size(vertices)), 0);
}
