#include "statue.h"
#include "rapidobj.hpp"
#include <winrt/base.h>
#define NOMINMAX
#include <d3d11.h>
#include <SimpleMath.h>
#include "graphics.h"
#include "camera.h"
#include "wavefront.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace
{
	const D3D11_INPUT_ELEMENT_DESC g_inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const UINT g_numInputElements = static_cast<unsigned>(std::size(g_inputElementDesc));

	struct Vertex
	{
		[[maybe_unused]] XMFLOAT3 position{};
		[[maybe_unused]] XMFLOAT3 normal{};
		[[maybe_unused]] XMFLOAT3 tangent{};
		[[maybe_unused]] XMFLOAT3 bitangent{};
		[[maybe_unused]] XMFLOAT2 texcoord{};
	};

	struct ConstantBuffer
	{
		[[maybe_unused]] XMMATRIX world{};
		[[maybe_unused]] XMMATRIX view{};
		[[maybe_unused]] XMMATRIX projection{};
		[[maybe_unused]] XMFLOAT3 lightDirection{};
		[[maybe_unused]] XMFLOAT3 cameraPosition{};
	};
}

class StatueImpl
{
public:
	explicit StatueImpl(std::shared_ptr<Graphics> pGraphics) : m_pGraphics{ pGraphics } {}
	StatueImpl(const StatueImpl&) = delete;
	StatueImpl(StatueImpl&&) = delete;
	StatueImpl& operator=(const StatueImpl&) = delete;
	StatueImpl& operator=(StatueImpl&&) = delete;
	~StatueImpl() = default;

	[[nodiscard]] bool loadWfo();
	void update(float tick);
	void draw(const Camera& camera) const;

private:
	std::shared_ptr<Graphics> m_pGraphics{ nullptr };
	winrt::com_ptr<ID3D11Buffer> m_pVertexBuffer{ nullptr };
	winrt::com_ptr<ID3D11Buffer> m_pIndexBuffer{ nullptr };
	winrt::com_ptr<ID3D11Buffer> m_pConstantBuffer{ nullptr };
	winrt::com_ptr<ID3D11VertexShader> m_pVertexShader{ nullptr };
	winrt::com_ptr<ID3D11InputLayout> m_pInputLayout{ nullptr };
	winrt::com_ptr<ID3D11PixelShader> m_pPixelShader{ nullptr };
	winrt::com_ptr<ID3D11SamplerState> m_pSamplerState{ nullptr };
	winrt::com_ptr<ID3D11ShaderResourceView> m_pTextureView[2]{ nullptr, nullptr };
	UINT m_numIndices{ 0 };
	float m_worldAngle{ 0 };
};

bool StatueImpl::loadWfo()
{
	FastVector<TbnVertex> vertices;
	FastVector<DWORD> indices;
	Vector4 sphere;
	if (!loadTbnObject("res\\statue\\statue.obj", vertices, indices, sphere))
		return false;

	if (!m_pGraphics->loadVertexBuffer(vertices.data(), static_cast<UINT>(vertices.size()), static_cast<UINT>(sizeof(Vertex)), m_pVertexBuffer.put()))
		return false;

	m_numIndices = static_cast<UINT>(indices.size());
	if (!m_pGraphics->loadIndexBuffer(indices.data(), m_numIndices, static_cast<UINT>(sizeof(DWORD)), m_pIndexBuffer.put()))
		return false;

	if (!m_pGraphics->createConstantBuffer(sizeof(ConstantBuffer), m_pConstantBuffer.put()))
		return false;

	if (!m_pGraphics->loadVertexShader(L"res\\statue_vs.cso", m_pVertexShader.put(), g_inputElementDesc, g_numInputElements, m_pInputLayout.put()))
		return false;

	if (!m_pGraphics->loadPixelShader(L"res\\statue_ps.cso", m_pPixelShader.put()))
		return false;

	const D3D11_SAMPLER_DESC samplerDesc
	{
		.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
		.AddressU = D3D11_TEXTURE_ADDRESS_BORDER,
		.AddressV = D3D11_TEXTURE_ADDRESS_BORDER,
		.AddressW = D3D11_TEXTURE_ADDRESS_BORDER,
		.MipLODBias = 0,
		.MaxAnisotropy = 0,
		.ComparisonFunc = D3D11_COMPARISON_NEVER,
		.BorderColor = { 0, 0, 0, 0 },
		.MinLOD = 0,
		.MaxLOD = 0,
	};

	HRESULT hr = m_pGraphics->getDevice()->CreateSamplerState(&samplerDesc, m_pSamplerState.put());
	if (FAILED(hr))
		return false;

	if (!m_pGraphics->loadDDSTexture(L"res\\statue\\statue_texture_TGA_DXT1_1.DDS", m_pTextureView[0].put()))
		return false;

	if (!m_pGraphics->loadWICTexture(L"res\\statue\\statue_normal.png", m_pTextureView[1].put()))
		return false;

	return true;
}

void StatueImpl::update(const float tick)
{
	m_worldAngle = (m_worldAngle < 2 * XM_PI) ? m_worldAngle + XM_PI * 0.005f * tick : 0;
}

void StatueImpl::draw(const Camera& camera) const
{
	const auto pDeviceContext = m_pGraphics->getDeviceContext();
	
	const Matrix world = Matrix::CreateScale(0.2f) * Matrix::CreateRotationY(m_worldAngle) * Matrix::CreateTranslation(0, -10, 5);

	const ConstantBuffer constantBuffer
	{
		.world = world.Transpose(),
		.view = camera.getView().Transpose(),
		.projection = camera.getProjection().Transpose(),
		.lightDirection = { 0.577f, 0.577f, -0.577f },
		.cameraPosition = camera.getPosition(),
	};

	pDeviceContext->UpdateSubresource(m_pConstantBuffer.get(), 0, nullptr, &constantBuffer, 0, 0);

	const UINT numConstantBuffers = 1;
	ID3D11Buffer* const constantBuffers[numConstantBuffers]{ m_pConstantBuffer.get() };

	pDeviceContext->VSSetConstantBuffers(0, numConstantBuffers, constantBuffers);
	pDeviceContext->VSSetShader(m_pVertexShader.get(), nullptr, 0);

	pDeviceContext->PSSetConstantBuffers(0, numConstantBuffers, constantBuffers);
	pDeviceContext->PSSetShader(m_pPixelShader.get(), nullptr, 0);

	const UINT numSamplerStates = 1;
	ID3D11SamplerState* const samplerStates[numSamplerStates]{ m_pSamplerState.get() };
	pDeviceContext->PSSetSamplers(0, 1, samplerStates);

	const UINT numTextureViews = 2;
	ID3D11ShaderResourceView* const textureViews[numTextureViews]{ m_pTextureView[0].get(), m_pTextureView[1].get() };
	pDeviceContext->PSSetShaderResources(0, numTextureViews, textureViews);

	const UINT numVertexBuffers = 1;
	const UINT vertexOffsets[numVertexBuffers]{ 0 };
	const UINT vertexStrides[numVertexBuffers]{ sizeof(Vertex) };
	ID3D11Buffer* const vertexBuffers[numVertexBuffers]{ m_pVertexBuffer.get() };
	pDeviceContext->IASetVertexBuffers(0, numVertexBuffers, vertexBuffers, vertexStrides, vertexOffsets);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer.get(), DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetInputLayout(m_pInputLayout.get());

	pDeviceContext->DrawIndexed(m_numIndices, 0, 0);
}

Statue::Statue(std::shared_ptr<Graphics> pGraphics)
	: m_pImpl{ std::make_unique<StatueImpl>(pGraphics) }
{
}

Statue::~Statue() = default;

bool Statue::load() const
{
	return m_pImpl->loadWfo();
}

void Statue::update(const float tick) const
{
	m_pImpl->update(tick);
}

void Statue::draw(const Camera& camera) const
{
	m_pImpl->draw(camera);
}
