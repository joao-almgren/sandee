#include "graphicstest.h"
#include <d3dcompiler.h>

namespace
{
	using namespace DirectX;

	const D3D11_INPUT_ELEMENT_DESC g_inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const UINT g_numInputElements = static_cast<unsigned>(std::size(g_inputElementDesc));

	struct Vertex
	{
		[[maybe_unused]] XMFLOAT3 position{};
		[[maybe_unused]] XMFLOAT3 normal;
		[[maybe_unused]] XMFLOAT4 color{};
		[[maybe_unused]] XMFLOAT2 uv{};
	};
	
	const Vertex g_vertices[]
	{
		{ XMFLOAT3(-1,  1, -1), XMFLOAT3( 0,  1,  0), XMFLOAT4(1, 0, 0, 1), XMFLOAT2(1, 0) },
		{ XMFLOAT3( 1,  1, -1), XMFLOAT3( 0,  1,  0), XMFLOAT4(0, 1, 0, 1), XMFLOAT2(0, 0) },
		{ XMFLOAT3( 1,  1,  1), XMFLOAT3( 0,  1,  0), XMFLOAT4(0, 0, 1, 1), XMFLOAT2(0, 1) },
		{ XMFLOAT3(-1,  1,  1), XMFLOAT3( 0,  1,  0), XMFLOAT4(0, 0, 0, 1), XMFLOAT2(1, 1) },

		{ XMFLOAT3(-1, -1, -1), XMFLOAT3( 0, -1,  0), XMFLOAT4(1, 0, 0, 1), XMFLOAT2(0, 0) },
		{ XMFLOAT3( 1, -1, -1), XMFLOAT3( 0, -1,  0), XMFLOAT4(0, 1, 0, 1), XMFLOAT2(1, 0) },
		{ XMFLOAT3( 1, -1,  1), XMFLOAT3( 0, -1,  0), XMFLOAT4(0, 0, 1, 1), XMFLOAT2(1, 1) },
		{ XMFLOAT3(-1, -1,  1), XMFLOAT3( 0, -1,  0), XMFLOAT4(0, 0, 0, 1), XMFLOAT2(0, 1) },

		{ XMFLOAT3(-1, -1,  1), XMFLOAT3(-1,  0,  0), XMFLOAT4(1, 0, 0, 1), XMFLOAT2(0, 1) },
		{ XMFLOAT3(-1, -1, -1), XMFLOAT3(-1,  0,  0), XMFLOAT4(0, 1, 0, 1), XMFLOAT2(1, 1) },
		{ XMFLOAT3(-1,  1, -1), XMFLOAT3(-1,  0,  0), XMFLOAT4(0, 0, 1, 1), XMFLOAT2(1, 0) },
		{ XMFLOAT3(-1,  1,  1), XMFLOAT3(-1,  0,  0), XMFLOAT4(0, 0, 0, 1), XMFLOAT2(0, 0) },

		{ XMFLOAT3( 1, -1,  1), XMFLOAT3( 1,  0,  0), XMFLOAT4(1, 0, 0, 1), XMFLOAT2(1, 1) },
		{ XMFLOAT3( 1, -1, -1), XMFLOAT3( 1,  0,  0), XMFLOAT4(0, 1, 0, 1), XMFLOAT2(0, 1) },
		{ XMFLOAT3( 1,  1, -1), XMFLOAT3( 1,  0,  0), XMFLOAT4(0, 0, 1, 1), XMFLOAT2(0, 0) },
		{ XMFLOAT3( 1,  1,  1), XMFLOAT3( 1,  0,  0), XMFLOAT4(0, 0, 0, 1), XMFLOAT2(1, 0) },

		{ XMFLOAT3(-1, -1, -1), XMFLOAT3( 0,  0, -1), XMFLOAT4(1, 0, 0, 1), XMFLOAT2(0, 1) },
		{ XMFLOAT3( 1, -1, -1), XMFLOAT3( 0,  0, -1), XMFLOAT4(0, 1, 0, 1), XMFLOAT2(1, 1) },
		{ XMFLOAT3( 1,  1, -1), XMFLOAT3( 0,  0, -1), XMFLOAT4(0, 0, 1, 1), XMFLOAT2(1, 0) },
		{ XMFLOAT3(-1,  1, -1), XMFLOAT3( 0,  0, -1), XMFLOAT4(0, 0, 0, 1), XMFLOAT2(0, 0) },

		{ XMFLOAT3(-1, -1,  1), XMFLOAT3( 0,  0,  1), XMFLOAT4(1, 0, 0, 1), XMFLOAT2(1, 1) },
		{ XMFLOAT3( 1, -1,  1), XMFLOAT3( 0,  0,  1), XMFLOAT4(0, 1, 0, 1), XMFLOAT2(0, 1) },
		{ XMFLOAT3( 1,  1,  1), XMFLOAT3( 0,  0,  1), XMFLOAT4(0, 0, 1, 1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-1,  1,  1), XMFLOAT3( 0,  0,  1), XMFLOAT4(0, 0, 0, 1), XMFLOAT2(1, 0) },
	};

	const WORD g_indices[]
	{
		3, 1, 0,
		2, 1, 3,

		6, 4, 5,
		7, 4, 6,

		11, 9, 8,
		10, 9, 11,

		14, 12, 13,
		15, 12, 14,

		19, 17, 16,
		18, 17, 19,

		22, 20, 21,
		23, 20, 22,
	};

	const UINT g_numIndices = static_cast<unsigned>(std::size(g_indices));

	struct ConstantBuffer
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
		XMFLOAT3 lightDir;
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

	D3D11_SAMPLER_DESC samplerDesc =
	{
		.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
		.AddressU = D3D11_TEXTURE_ADDRESS_BORDER,
		.AddressV = D3D11_TEXTURE_ADDRESS_BORDER,
		.AddressW = D3D11_TEXTURE_ADDRESS_BORDER,
		.ComparisonFunc = D3D11_COMPARISON_NEVER,
		.BorderColor = { 0, 0, 0, 0 },
	};

	hr = pDevice->CreateSamplerState(&samplerDesc, pSamplerState.put());
	if (FAILED(hr))
		return false;

	if (!pGraphics->loadTexture("cliff_03_v1.tga", pTexture.put(), pTextureView.put()))
		return false;

	return true;
}

void GraphicsTest::draw(const winrt::com_ptr<ID3D11DeviceContext> pDeviceContext, const XMMATRIX & view, const XMMATRIX & proj) const
{
	static float t = 0;
	t = (t < 2 * XM_PI) ? t + XM_PI * 0.002f : 0;

	XMMATRIX world = XMMatrixRotationRollPitchYaw(t, t, 0);

	ConstantBuffer constantBuffer
	{
		.world = XMMatrixTranspose(world),
		.view = XMMatrixTranspose(view),
		.proj = XMMatrixTranspose(proj),
		.lightDir = XMFLOAT3(-0.577f, 0.577f, -0.577f),
	};

	pDeviceContext->UpdateSubresource(pConstantBuffer.get(), 0, nullptr, &constantBuffer, 0, 0);

	const UINT numConstantBuffers = 1;
	ID3D11Buffer* const constantBuffers[numConstantBuffers]{ pConstantBuffer.get() };

	pDeviceContext->VSSetConstantBuffers(0, numConstantBuffers, constantBuffers);
	pDeviceContext->VSSetShader(pVertexShader.get(), nullptr, 0);

	pDeviceContext->PSSetConstantBuffers(0, numConstantBuffers, constantBuffers);
	pDeviceContext->PSSetShader(pPixelShader.get(), nullptr, 0);

	const UINT numSamplerStates = 1;
	ID3D11SamplerState* const samplerStates[numSamplerStates]{ pSamplerState.get() };
	pDeviceContext->PSSetSamplers(0, 1, samplerStates);

	const UINT numTextureViews = 1;
	ID3D11ShaderResourceView* const textureViews[numTextureViews]{ pTextureView.get() };
	pDeviceContext->PSSetShaderResources(0, 1, textureViews);

	const UINT numVertexBuffers = 1;
	const UINT vertexOffsets[numVertexBuffers]{ 0 };
	const UINT vertexStrides[numVertexBuffers]{ sizeof(Vertex) };
	ID3D11Buffer* const vertexBuffers[numVertexBuffers]{ pVertexBuffer.get() };
	pDeviceContext->IASetVertexBuffers(0, numVertexBuffers, vertexBuffers, vertexStrides, vertexOffsets);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetIndexBuffer(pIndexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);
	pDeviceContext->IASetInputLayout(pInputLayout.get());

	pDeviceContext->DrawIndexed(g_numIndices, 0, 0);
}
