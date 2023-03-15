#include "lwotest.h"
#include "rapidobj.hpp"
#include <winrt/base.h>
#define NOMINMAX
#include <d3d11.h>

namespace
{
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

	[[nodiscard]] bool load();
	void draw(const Camera& camera) const;
	void update(float tick);

private:
	std::shared_ptr<Graphics> m_pGraphics{ nullptr };
	winrt::com_ptr<ID3D11Buffer> m_pVertexBuffer{ nullptr };
	winrt::com_ptr<ID3D11Buffer> m_pIndexBuffer{ nullptr };
	winrt::com_ptr<ID3D11Buffer> m_pConstantBuffer{ nullptr };
	winrt::com_ptr<ID3D11VertexShader> m_pVertexShader{ nullptr };
	winrt::com_ptr<ID3D11InputLayout> m_pInputLayout{ nullptr };
	winrt::com_ptr<ID3D11PixelShader> m_pPixelShader{ nullptr };
};

bool LwoTestImpl::load()
{
	return true;
}

void LwoTestImpl::draw(const Camera& camera) const
{
}

void LwoTestImpl::update(const float /*tick*/)
{
}

LwoTest::LwoTest(std::shared_ptr<Graphics> pGraphics)
	: m_pImpl{ std::make_unique<LwoTestImpl>(pGraphics) }
{
}

LwoTest::~LwoTest() = default;

bool LwoTest::load() const
{
	return m_pImpl->load();
}

void LwoTest::draw(const Camera& camera) const
{
	m_pImpl->draw(camera);
}

void LwoTest::update(const float tick) const
{
	m_pImpl->update(tick);
}
