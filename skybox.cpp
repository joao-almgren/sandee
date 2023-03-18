#include "skybox.h"
#include <winrt/base.h>
#define NOMINMAX
#include <d3d11.h>
#include <d3dcompiler.h>
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	const UINT g_numInputElements = static_cast<unsigned>(std::size(g_inputElementDesc));

	struct Vertex
	{
		[[maybe_unused]] XMFLOAT3 position{};
		[[maybe_unused]] XMFLOAT2 uv{};
	};

	const Vertex g_vertices[]
	{
		{ { -0.5f,  0.5f, -0.5f }, { 0, 0 } },
		{ {  0.5f,  0.5f, -0.5f }, { 1, 0 } },
		{ { -0.5f,  0.5f,  0.5f }, { 0, 1 } },
		{ {  0.5f,  0.5f,  0.5f }, { 1, 1 } },

		{ { -0.5f,  0.5f,  0.5f }, { 0, 0 } },
		{ {  0.5f,  0.5f,  0.5f }, { 1, 0 } },
		{ { -0.5f, -0.5f,  0.5f }, { 0, 1 } },
		{ {  0.5f, -0.5f,  0.5f }, { 1, 1 } },

		{ {  0.5f,  0.5f,  0.5f }, { 0, 0 } },
		{ {  0.5f,  0.5f, -0.5f }, { 1, 0 } },
		{ {  0.5f, -0.5f,  0.5f }, { 0, 1 } },
		{ {  0.5f, -0.5f, -0.5f }, { 1, 1 } },

		{ {  0.5f,  0.5f, -0.5f }, { 0, 0 } },
		{ { -0.5f,  0.5f, -0.5f }, { 1, 0 } },
		{ {  0.5f, -0.5f, -0.5f }, { 0, 1 } },
		{ { -0.5f, -0.5f, -0.5f }, { 1, 1 } },

		{ { -0.5f,  0.5f, -0.5f }, { 0, 0 } },
		{ { -0.5f,  0.5f,  0.5f }, { 1, 0 } },
		{ { -0.5f, -0.5f, -0.5f }, { 0, 1 } },
		{ { -0.5f, -0.5f,  0.5f }, { 1, 1 } },
	};

	const UINT g_numVertices = static_cast<unsigned>(std::size(g_vertices));

	struct ConstantBuffer
	{
		[[maybe_unused]] XMMATRIX world{};
		[[maybe_unused]] XMMATRIX view{};
		[[maybe_unused]] XMMATRIX projection{};
	};
}

class SkyboxImpl
{
public:
	explicit SkyboxImpl(std::shared_ptr<Graphics> pGraphics) : m_pGraphics{ pGraphics } {}
	SkyboxImpl(const SkyboxImpl&) = delete;
	SkyboxImpl(SkyboxImpl&&) = delete;
	SkyboxImpl& operator=(const SkyboxImpl&) = delete;
	SkyboxImpl& operator=(SkyboxImpl&&) = delete;
	~SkyboxImpl() = default;

	[[nodiscard]] bool load();
	void update(float tick);
	void draw(const Camera& camera) const;

private:
	std::shared_ptr<Graphics> m_pGraphics{ nullptr };
	winrt::com_ptr<ID3D11RasterizerState> m_pRasterizerState{ nullptr };
	winrt::com_ptr<ID3D11Buffer> m_pVertexBuffer{ nullptr };
	winrt::com_ptr<ID3D11Buffer> m_pConstantBuffer{ nullptr };
	winrt::com_ptr<ID3D11VertexShader> m_pVertexShader{ nullptr };
	winrt::com_ptr<ID3D11InputLayout> m_pInputLayout{ nullptr };
	winrt::com_ptr<ID3D11PixelShader> m_pPixelShader{ nullptr };
	winrt::com_ptr<ID3D11SamplerState> m_pSamplerState{ nullptr };
	winrt::com_ptr<ID3D11ShaderResourceView> m_pTextureView[5]{ nullptr, nullptr, nullptr, nullptr, nullptr };
};

bool SkyboxImpl::load()
{
	const auto pDevice = m_pGraphics->getDevice();

	const D3D11_RASTERIZER_DESC rasterizerDesc
	{
		.FillMode = D3D11_FILL_SOLID,
		.CullMode = D3D11_CULL_NONE,
		.FrontCounterClockwise = TRUE,
		.DepthBias = 0,
		.DepthBiasClamp = 0.0f,
		.SlopeScaledDepthBias = 0.0f,
		.DepthClipEnable = FALSE,
		.ScissorEnable = FALSE,
		.MultisampleEnable = FALSE,
		.AntialiasedLineEnable = FALSE,
	};

	HRESULT hr = pDevice->CreateRasterizerState(&rasterizerDesc, m_pRasterizerState.put());
	if (FAILED(hr))
		return false;

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

	hr = pDevice->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, m_pVertexBuffer.put());
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

	hr = pDevice->CreateBuffer(&constantBufferDesc, nullptr, m_pConstantBuffer.put());
	if (FAILED(hr))
		return false;

	winrt::com_ptr<ID3DBlob> pVertexBlob = nullptr;
	hr = D3DReadFileToBlob(L"res\\vertexskybox.cso", pVertexBlob.put());
	if (FAILED(hr))
		return false;

	hr = pDevice->CreateVertexShader(pVertexBlob->GetBufferPointer(), pVertexBlob->GetBufferSize(), nullptr, m_pVertexShader.put());
	if (FAILED(hr))
		return false;

	hr = pDevice->CreateInputLayout(g_inputElementDesc, g_numInputElements, pVertexBlob->GetBufferPointer(), pVertexBlob->GetBufferSize(), m_pInputLayout.put());
	if (FAILED(hr))
		return false;

	winrt::com_ptr<ID3DBlob> pPixelBlob = nullptr;
	hr = D3DReadFileToBlob(L"res\\pixelskybox.cso", pPixelBlob.put());
	if (FAILED(hr))
		return false;

	hr = pDevice->CreatePixelShader(pPixelBlob->GetBufferPointer(), pPixelBlob->GetBufferSize(), nullptr, m_pPixelShader.put());
	if (FAILED(hr))
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

	hr = pDevice->CreateSamplerState(&samplerDesc, m_pSamplerState.put());
	if (FAILED(hr))
		return false;

	if (!m_pGraphics->loadDDSTexture(L"res\\envmap_miramar\\results\\miramar_up_tga_dxt1_1.dds", m_pTextureView[0].put())
		|| !m_pGraphics->loadDDSTexture(L"res\\envmap_miramar\\results\\miramar_rt_tga_dxt1_1.dds", m_pTextureView[1].put())
		|| !m_pGraphics->loadDDSTexture(L"res\\envmap_miramar\\results\\miramar_ft_tga_dxt1_1.dds", m_pTextureView[2].put())
		|| !m_pGraphics->loadDDSTexture(L"res\\envmap_miramar\\results\\miramar_lf_tga_dxt1_1.dds", m_pTextureView[3].put())
		|| !m_pGraphics->loadDDSTexture(L"res\\envmap_miramar\\results\\miramar_bk_tga_dxt1_1.dds", m_pTextureView[4].put()))
		return false;

	return true;
}

void SkyboxImpl::update(const float /*tick*/)
{
}

void SkyboxImpl::draw(const Camera& camera) const
{
	const auto pDeviceContext = m_pGraphics->getDeviceContext();
	pDeviceContext->RSSetState(m_pRasterizerState.get());

	const Matrix world = Matrix::CreateScale(100, 100, 100) * Matrix::CreateTranslation(camera.getPosition());

	const ConstantBuffer constantBuffer
	{
		.world = world.Transpose(),
		.view = camera.getView().Transpose(),
		.projection = camera.getProjection().Transpose(),
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

	const UINT numVertexBuffers = 1;
	const UINT vertexOffsets[numVertexBuffers]{ 0 };
	const UINT vertexStrides[numVertexBuffers]{ sizeof(Vertex) };
	ID3D11Buffer* const vertexBuffers[numVertexBuffers]{ m_pVertexBuffer.get() };
	pDeviceContext->IASetVertexBuffers(0, numVertexBuffers, vertexBuffers, vertexStrides, vertexOffsets);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pDeviceContext->IASetInputLayout(m_pInputLayout.get());

	for (int s = 0; s < 5; s++)
	{
		const UINT numTextureViews = 1;
		ID3D11ShaderResourceView* const textureViews[numTextureViews]{ m_pTextureView[s].get() };
		pDeviceContext->PSSetShaderResources(0, numTextureViews, textureViews);

		pDeviceContext->Draw(4, s * 4);
	}

	m_pGraphics->resetRasterizer();
}

Skybox::Skybox(std::shared_ptr<Graphics> pGraphics)
	: m_pImpl{ std::make_unique<SkyboxImpl>(pGraphics) }
{
}

Skybox::~Skybox() = default;

bool Skybox::load() const
{
	return m_pImpl->load();
}

void Skybox::update(const float tick) const
{
	m_pImpl->update(tick);
}

void Skybox::draw(const Camera& camera) const
{
	m_pImpl->draw(camera);
}
