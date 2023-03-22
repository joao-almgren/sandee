#include "lwotest.h"
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

class LwoTestImpl
{
public:
	explicit LwoTestImpl(std::shared_ptr<Graphics> pGraphics) : m_pGraphics{ pGraphics } {}
	LwoTestImpl(const LwoTestImpl&) = delete;
	LwoTestImpl(LwoTestImpl&&) = delete;
	LwoTestImpl& operator=(const LwoTestImpl&) = delete;
	LwoTestImpl& operator=(LwoTestImpl&&) = delete;
	~LwoTestImpl() = default;

	[[nodiscard]] bool loadRapid();
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
	UINT m_numIndices{ 0 };
	float m_worldAngle{ 0 };
};

bool LwoTestImpl::loadRapid()
{
	rapidobj::Result result = rapidobj::ParseFile("res\\bunny.obj");
	if (result.error)
		return false;

	if (!rapidobj::Triangulate(result))
		return false;

	const UINT numVertices = static_cast<UINT>(result.attributes.positions.size() / 3);
	const std::unique_ptr<Vertex> vertices(new Vertex[numVertices]);

	Vertex * pVertices = vertices.get();
	for (size_t i = 0; i < static_cast<size_t>(numVertices) * 3; i += 3)
	{
		const size_t index = i / 3;
		pVertices[index].position.x = result.attributes.positions[i];
		pVertices[index].position.y = result.attributes.positions[i + 1];
		pVertices[index].position.z = result.attributes.positions[i + 2];
	}

	m_numIndices = static_cast<UINT>(result.shapes[0].mesh.indices.size());
	const std::unique_ptr<DWORD> indices(new DWORD[m_numIndices]);

	DWORD * pIndices = indices.get();
	for (size_t i = 0; i < m_numIndices; i++)
	{
		pIndices[i] = result.shapes[0].mesh.indices[i].position_index;

		const size_t normalIndex = 3 * static_cast<size_t>(result.shapes[0].mesh.indices[i].normal_index);
		pVertices[pIndices[i]].normal.x = result.attributes.normals[normalIndex];
		pVertices[pIndices[i]].normal.y = result.attributes.normals[normalIndex + 1];
		pVertices[pIndices[i]].normal.z = result.attributes.normals[normalIndex + 2];
	}

	if (!m_pGraphics->loadVertexBuffer(vertices.get(), numVertices, static_cast<UINT>(sizeof(Vertex)), m_pVertexBuffer.put()))
		return false;

	if (!m_pGraphics->loadIndexBuffer(indices.get(), m_numIndices, static_cast<UINT>(sizeof(DWORD)), m_pIndexBuffer.put()))
		return false;

	if (!m_pGraphics->createConstantBuffer(sizeof(ConstantBuffer), m_pConstantBuffer.put()))
		return false;

	if (!m_pGraphics->loadVertexShader(L"res\\lwotest_vs.cso", m_pVertexShader.put(), g_inputElementDesc, g_numInputElements, m_pInputLayout.put()))
		return false;

	if (!m_pGraphics->loadPixelShader(L"res\\lwotest_ps.cso", m_pPixelShader.put()))
		return false;

	return true;
}

bool LwoTestImpl::loadWfo()
{
	FastVector<TbnVertex> vertices;
	FastVector<DWORD> indices;
	Vector4 sphere;
	if (!loadTbnObject("res\\bunny.obj", vertices, indices, sphere))
		return false;

	if (!m_pGraphics->loadVertexBuffer(vertices.data(), static_cast<UINT>(vertices.size()), static_cast<UINT>(sizeof(Vertex)), m_pVertexBuffer.put()))
		return false;

	m_numIndices = static_cast<UINT>(indices.size());
	if (!m_pGraphics->loadIndexBuffer(indices.data(), m_numIndices, static_cast<UINT>(sizeof(DWORD)), m_pIndexBuffer.put()))
		return false;

	if (!m_pGraphics->createConstantBuffer(sizeof(ConstantBuffer), m_pConstantBuffer.put()))
		return false;

	if (!m_pGraphics->loadVertexShader(L"res\\lwotest_vs.cso", m_pVertexShader.put(), g_inputElementDesc, g_numInputElements, m_pInputLayout.put()))
		return false;

	if (!m_pGraphics->loadPixelShader(L"res\\lwotest_ps.cso", m_pPixelShader.put()))
		return false;

	return true;
}

void LwoTestImpl::update(const float tick)
{
	m_worldAngle = (m_worldAngle < 2 * XM_PI) ? m_worldAngle + XM_PI * 0.005f * tick : 0;
}

void LwoTestImpl::draw(const Camera& camera) const
{
	const auto pDeviceContext = m_pGraphics->getDeviceContext();
	
	const Matrix world = Matrix::CreateScale(3) * Matrix::CreateRotationY(m_worldAngle) * Matrix::CreateTranslation(0, -2, 0);

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

LwoTest::LwoTest(std::shared_ptr<Graphics> pGraphics)
	: m_pImpl{ std::make_unique<LwoTestImpl>(pGraphics) }
{
}

LwoTest::~LwoTest() = default;

bool LwoTest::load() const
{
	return m_pImpl->loadWfo();
}

void LwoTest::update(const float tick) const
{
	m_pImpl->update(tick);
}

void LwoTest::draw(const Camera& camera) const
{
	m_pImpl->draw(camera);
}
