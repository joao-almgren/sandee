#include "graphicstest.h"
#include <d3dcompiler.h>
#include <directxmath.h>

namespace
{
	using namespace DirectX;

	const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const UINT numInputElements = static_cast<unsigned>(std::size(inputElementDesc));

	struct Vertex
	{
		[[maybe_unused]] XMFLOAT3 position;
		[[maybe_unused]] XMFLOAT4 color;
	};
	
	const Vertex vertices[]
	{
		{ .position = XMFLOAT3(  0.0f,  0.5f, 0 ), .color = XMFLOAT4( 1, 1, 0, 1 ) },
		{ .position = XMFLOAT3(  0.5f, -0.5f, 0 ), .color = XMFLOAT4( 1, 0, 1, 1 ) },
		{ .position = XMFLOAT3( -0.5f, -0.5f, 0 ), .color = XMFLOAT4( 0, 1, 1, 1 ) },
	};

	const WORD indices[]
	{
		0, 1, 2,
	};
}

bool GraphicsTest::load(const winrt::com_ptr<ID3D11Device> pDevice)
{
	const D3D11_BUFFER_DESC vertexBufferDesc
	{
		.ByteWidth = sizeof(vertices),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = sizeof(Vertex),
	};

	const D3D11_SUBRESOURCE_DATA vertexSubresourceData
	{
		.pSysMem = vertices,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hr = pDevice->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, pVertexBuffer.put());
	if (FAILED(hr))
		return false;

	const D3D11_BUFFER_DESC indexBufferDesc
	{
		.ByteWidth = sizeof(indices),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = sizeof(WORD),
	};

	const D3D11_SUBRESOURCE_DATA indexSubresourceData
	{
		.pSysMem = indices,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	hr = pDevice->CreateBuffer(&indexBufferDesc, &indexSubresourceData, pIndexBuffer.put());
	if (FAILED(hr))
		return false;

	const UINT hlslFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_OPTIMIZATION_LEVEL3;

	winrt::com_ptr<ID3DBlob> pVertexBlob = nullptr;
	//hr = D3DReadFileToBlob(L"vertextest.cso", pVertexBlob.put());
	hr = D3DCompileFromFile(L"vertextest.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", hlslFlags, 0, pVertexBlob.put(), nullptr);
	if (FAILED(hr))
		return false;

	hr = pDevice->CreateVertexShader(pVertexBlob->GetBufferPointer(), pVertexBlob->GetBufferSize(), nullptr, pVertexShader.put());
	if (FAILED(hr))
		return false;

	hr = pDevice->CreateInputLayout(inputElementDesc, numInputElements, pVertexBlob->GetBufferPointer(), pVertexBlob->GetBufferSize(), pInputLayout.put());
	if (FAILED(hr))
		return false;

	winrt::com_ptr<ID3DBlob> pPixelBlob = nullptr;
	//hr = D3DReadFileToBlob(L"pixeltest.cso", pPixelBlob.put());
	hr = D3DCompileFromFile(L"pixeltest.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", hlslFlags, 0, pPixelBlob.put(), nullptr);
	if (FAILED(hr))
		return false;

	hr = pDevice->CreatePixelShader(pPixelBlob->GetBufferPointer(), pPixelBlob->GetBufferSize(), nullptr, pPixelShader.put());
	if (FAILED(hr))
		return false;

	return true;
}

void GraphicsTest::draw(const winrt::com_ptr<ID3D11DeviceContext> pDeviceContext) const
{
	const UINT numBuffers = 1;
	const UINT vertexOffsets[numBuffers]{ 0 };
	const UINT vertexStrides[numBuffers]{ sizeof(Vertex) };
	ID3D11Buffer* const vertexBuffers[numBuffers]{ pVertexBuffer.get() };
	pDeviceContext->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, vertexOffsets);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetIndexBuffer(pIndexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);
	pDeviceContext->IASetInputLayout(pInputLayout.get());
	pDeviceContext->VSSetShader(pVertexShader.get(), nullptr, 0);
	pDeviceContext->PSSetShader(pPixelShader.get(), nullptr, 0);
	pDeviceContext->DrawIndexed(static_cast<unsigned>(std::size(indices)), 0, 0);
}
