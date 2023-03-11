#include "graphicstest.h"
#include <d3dcompiler.h>
#include <directxmath.h>

namespace
{
	using namespace DirectX;

	const D3D11_INPUT_ELEMENT_DESC g_inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const UINT g_numInputElements = static_cast<unsigned>(std::size(g_inputElementDesc));

	struct Vertex
	{
		[[maybe_unused]] XMFLOAT3 position{};
		[[maybe_unused]] XMFLOAT4 color{};
	};
	
	const Vertex g_vertices[]
	{
		{ .position = XMFLOAT3( -1,  1, 0 ), .color = XMFLOAT4( 1, 0, 0, 1 ) },
		{ .position = XMFLOAT3(  1,  1, 0 ), .color = XMFLOAT4( 0, 1, 0, 1 ) },
		{ .position = XMFLOAT3(  1, -1, 0 ), .color = XMFLOAT4( 0, 0, 1, 1 ) },
		{ .position = XMFLOAT3( -1, -1, 0 ), .color = XMFLOAT4( 1, 1, 0, 1 ) },
	};

	const WORD g_indices[]
	{
		0, 1, 2, 0, 2, 3,
	};

	const UINT g_numIndices = static_cast<unsigned>(std::size(g_indices));

	struct ConstantBuffer
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
	};
}

bool GraphicsTest::load(const winrt::com_ptr<ID3D11Device> pDevice)
{
	const D3D11_BUFFER_DESC vertexBufferDesc
	{
		.ByteWidth = sizeof(g_vertices),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = sizeof(Vertex),
	};

	const D3D11_SUBRESOURCE_DATA vertexSubresourceData
	{
		.pSysMem = g_vertices,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hr = pDevice->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, pVertexBuffer.put());
	if (FAILED(hr))
		return false;

	const D3D11_BUFFER_DESC indexBufferDesc
	{
		.ByteWidth = sizeof(g_indices),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = sizeof(WORD),
	};

	const D3D11_SUBRESOURCE_DATA indexSubresourceData
	{
		.pSysMem = g_indices,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	hr = pDevice->CreateBuffer(&indexBufferDesc, &indexSubresourceData, pIndexBuffer.put());
	if (FAILED(hr))
		return false;

	const D3D11_BUFFER_DESC constantBufferDesc
	{
		.ByteWidth = sizeof(ConstantBuffer),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = sizeof(ConstantBuffer),
	};

	hr = pDevice->CreateBuffer(&constantBufferDesc, nullptr, pConstantBuffer.put());
	if (FAILED(hr))
		return false;

	winrt::com_ptr<ID3DBlob> pVertexBlob = nullptr;
	hr = D3DReadFileToBlob(L"vertextest.cso", pVertexBlob.put());
	if (FAILED(hr))
		return false;

	hr = pDevice->CreateVertexShader(pVertexBlob->GetBufferPointer(), pVertexBlob->GetBufferSize(), nullptr, pVertexShader.put());
	if (FAILED(hr))
		return false;

	hr = pDevice->CreateInputLayout(g_inputElementDesc, g_numInputElements, pVertexBlob->GetBufferPointer(), pVertexBlob->GetBufferSize(), pInputLayout.put());
	if (FAILED(hr))
		return false;

	winrt::com_ptr<ID3DBlob> pPixelBlob = nullptr;
	hr = D3DReadFileToBlob(L"pixeltest.cso", pPixelBlob.put());
	if (FAILED(hr))
		return false;

	hr = pDevice->CreatePixelShader(pPixelBlob->GetBufferPointer(), pPixelBlob->GetBufferSize(), nullptr, pPixelShader.put());
	if (FAILED(hr))
		return false;

	return true;
}

void GraphicsTest::draw(const winrt::com_ptr<ID3D11DeviceContext> pDeviceContext, const int windowWidth, const int windowHeight) const
{
	static float t = 0;
	t = (t < 2 * XM_PI) ? t + XM_PI * 0.005f : 0;

	XMMATRIX world = XMMatrixRotationZ(t);

	XMVECTOR eyePos = XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f);
	XMVECTOR atPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR upDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(eyePos, atPos, upDir);

	XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV2, windowWidth / static_cast<float>(windowHeight), 0.1f, 1000.0f);

	ConstantBuffer constantBuffer
	{
		.world = XMMatrixTranspose(world),
		.view = XMMatrixTranspose(view),
		.proj = XMMatrixTranspose(proj),
	};

	pDeviceContext->UpdateSubresource(pConstantBuffer.get(), 0, nullptr, &constantBuffer, 0, 0);

	const UINT numVertexBuffers = 1;
	const UINT vertexOffsets[numVertexBuffers]{ 0 };
	const UINT vertexStrides[numVertexBuffers]{ sizeof(Vertex) };
	ID3D11Buffer* const vertexBuffers[numVertexBuffers]{ pVertexBuffer.get() };
	pDeviceContext->IASetVertexBuffers(0, numVertexBuffers, vertexBuffers, vertexStrides, vertexOffsets);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetIndexBuffer(pIndexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);
	pDeviceContext->IASetInputLayout(pInputLayout.get());

	const UINT numConstantBuffers = 1;
	ID3D11Buffer* const constantBuffers[numConstantBuffers]{ pConstantBuffer.get() };
	pDeviceContext->VSSetConstantBuffers(0, numConstantBuffers, constantBuffers);
	pDeviceContext->VSSetShader(pVertexShader.get(), nullptr, 0);
	pDeviceContext->PSSetShader(pPixelShader.get(), nullptr, 0);
	pDeviceContext->DrawIndexed(g_numIndices, 0, 0);
}
