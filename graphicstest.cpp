#include "graphicstest.h"
#include <winrt/base.h>
#define NOMINMAX
#include <d3d11.h>
#include <SimpleMath.h>
#include "graphics.h"
#include "camera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace
{
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
		[[maybe_unused]] XMFLOAT3 normal{};
		[[maybe_unused]] XMFLOAT4 color{};
		[[maybe_unused]] XMFLOAT2 uv{};
	};
	
	const Vertex g_vertices[]
	{
		{ {-1, 1,-1}, { 0, 1, 0}, {1, 0, 0, 1}, {1, 0} },
		{ { 1, 1,-1}, { 0, 1, 0}, {0, 1, 0, 1}, {0, 0} },
		{ { 1, 1, 1}, { 0, 1, 0}, {0, 0, 1, 1}, {0, 1} },
		{ {-1, 1, 1}, { 0, 1, 0}, {0, 0, 0, 1}, {1, 1} },

		{ {-1,-1,-1}, { 0,-1, 0}, {1, 0, 0, 1}, {0, 0} },
		{ { 1,-1,-1}, { 0,-1, 0}, {0, 1, 0, 1}, {1, 0} },
		{ { 1,-1, 1}, { 0,-1, 0}, {0, 0, 1, 1}, {1, 1} },
		{ {-1,-1, 1}, { 0,-1, 0}, {0, 0, 0, 1}, {0, 1} },

		{ {-1,-1, 1}, {-1, 0, 0}, {1, 0, 0, 1}, {0, 1} },
		{ {-1,-1,-1}, {-1, 0, 0}, {0, 1, 0, 1}, {1, 1} },
		{ {-1, 1,-1}, {-1, 0, 0}, {0, 0, 1, 1}, {1, 0} },
		{ {-1, 1, 1}, {-1, 0, 0}, {0, 0, 0, 1}, {0, 0} },

		{ { 1,-1, 1}, { 1, 0, 0}, {1, 0, 0, 1}, {1, 1} },
		{ { 1,-1,-1}, { 1, 0, 0}, {0, 1, 0, 1}, {0, 1} },
		{ { 1, 1,-1}, { 1, 0, 0}, {0, 0, 1, 1}, {0, 0} },
		{ { 1, 1, 1}, { 1, 0, 0}, {0, 0, 0, 1}, {1, 0} },

		{ {-1,-1,-1}, { 0, 0,-1}, {1, 0, 0, 1}, {0, 1} },
		{ { 1,-1,-1}, { 0, 0,-1}, {0, 1, 0, 1}, {1, 1} },
		{ { 1, 1,-1}, { 0, 0,-1}, {0, 0, 1, 1}, {1, 0} },
		{ {-1, 1,-1}, { 0, 0,-1}, {0, 0, 0, 1}, {0, 0} },

		{ {-1,-1, 1}, { 0, 0, 1}, {1, 0, 0, 1}, {1, 1} },
		{ { 1,-1, 1}, { 0, 0, 1}, {0, 1, 0, 1}, {0, 1} },
		{ { 1, 1, 1}, { 0, 0, 1}, {0, 0, 1, 1}, {0, 0} },
		{ {-1, 1, 1}, { 0, 0, 1}, {0, 0, 0, 1}, {1, 0} },
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
		[[maybe_unused]] XMMATRIX world{};
		[[maybe_unused]] XMMATRIX view{};
		[[maybe_unused]] XMMATRIX projection{};
		[[maybe_unused]] XMFLOAT3 lightDirection{};
	};
}

class GraphicsTestImpl
{
public:
	explicit GraphicsTestImpl(std::shared_ptr<Graphics> pGraphics) : m_pGraphics{ pGraphics } {}
	GraphicsTestImpl(const GraphicsTestImpl&) = delete;
	GraphicsTestImpl(GraphicsTestImpl&&) = delete;
	GraphicsTestImpl& operator=(const GraphicsTestImpl&) = delete;
	GraphicsTestImpl& operator=(GraphicsTestImpl&&) = delete;
	~GraphicsTestImpl() = default;

	[[nodiscard]] bool load();
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
	winrt::com_ptr<ID3D11ShaderResourceView> m_pTextureView{ nullptr };
	float m_worldAngle{ 0 };
};

bool GraphicsTestImpl::load()
{
	const auto pDevice = m_pGraphics->getDevice();

	if (!m_pGraphics->loadVertexBuffer(g_vertices, static_cast<UINT>(std::size(g_vertices)), sizeof(Vertex), m_pVertexBuffer.put()))
		return false;

	if (!m_pGraphics->loadIndexBuffer(g_indices, g_numIndices, sizeof(WORD), m_pIndexBuffer.put()))
		return false;

	if (!m_pGraphics->createConstantBuffer(sizeof(ConstantBuffer), m_pConstantBuffer.put()))
		return false;

	if (!m_pGraphics->loadVertexShader(L"res\\graphicstest_vs.cso", m_pVertexShader.put(), g_inputElementDesc, g_numInputElements, m_pInputLayout.put()))
		return false;

	if (!m_pGraphics->loadPixelShader(L"res\\graphicstest_ps.cso", m_pPixelShader.put()))
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

	HRESULT hr = pDevice->CreateSamplerState(&samplerDesc, m_pSamplerState.put());
	if (FAILED(hr))
		return false;

	if (!m_pGraphics->loadTexture("res\\cliff_03_v1.tga", m_pTextureView.put()))
		return false;

	return true;
}

void GraphicsTestImpl::update(const float tick)
{
	m_worldAngle = (m_worldAngle < 2 * XM_PI) ? m_worldAngle + XM_PI * 0.005f * tick : 0;
}

void GraphicsTestImpl::draw(const Camera & camera) const
{
	const auto pDeviceContext = m_pGraphics->getDeviceContext();

	const Matrix world = Matrix::CreateTranslation(5, 0, 0) * Matrix::CreateFromYawPitchRoll(m_worldAngle, m_worldAngle, m_worldAngle);

	const ConstantBuffer constantBuffer
	{
		.world = world.Transpose(),
		.view = camera.getView().Transpose(),
		.projection = camera.getProjection().Transpose(),
		.lightDirection = { 0.577f, 0.577f, -0.577f },
	};

	pDeviceContext->UpdateSubresource(m_pConstantBuffer.get(), 0, nullptr, &constantBuffer, 0, 0);

	const UINT numConstantBuffers = 1;
	ID3D11Buffer * const constantBuffers[numConstantBuffers]{ m_pConstantBuffer.get() };

	pDeviceContext->VSSetConstantBuffers(0, numConstantBuffers, constantBuffers);
	pDeviceContext->VSSetShader(m_pVertexShader.get(), nullptr, 0);

	pDeviceContext->PSSetConstantBuffers(0, numConstantBuffers, constantBuffers);
	pDeviceContext->PSSetShader(m_pPixelShader.get(), nullptr, 0);

	const UINT numSamplerStates = 1;
	ID3D11SamplerState* const samplerStates[numSamplerStates]{ m_pSamplerState.get() };
	pDeviceContext->PSSetSamplers(0, 1, samplerStates);

	const UINT numTextureViews = 1;
	ID3D11ShaderResourceView* const textureViews[numTextureViews]{ m_pTextureView.get() };
	pDeviceContext->PSSetShaderResources(0, numTextureViews, textureViews);

	const UINT numVertexBuffers = 1;
	const UINT vertexOffsets[numVertexBuffers]{ 0 };
	const UINT vertexStrides[numVertexBuffers]{ sizeof(Vertex) };
	ID3D11Buffer * const vertexBuffers[numVertexBuffers]{ m_pVertexBuffer.get() };
	pDeviceContext->IASetVertexBuffers(0, numVertexBuffers, vertexBuffers, vertexStrides, vertexOffsets);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);
	pDeviceContext->IASetInputLayout(m_pInputLayout.get());

	pDeviceContext->DrawIndexed(g_numIndices, 0, 0);
}

GraphicsTest::GraphicsTest(std::shared_ptr<Graphics> pGraphics)
	: m_pImpl{ std::make_unique<GraphicsTestImpl>(pGraphics) }
{
}

GraphicsTest::~GraphicsTest() = default;

bool GraphicsTest::load() const
{
	return m_pImpl->load();
}

void GraphicsTest::update(const float tick) const
{
	m_pImpl->update(tick);
}

void GraphicsTest::draw(const Camera& camera) const
{
	m_pImpl->draw(camera);
}
