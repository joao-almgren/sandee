#pragma once
#include <d3d11.h>
#include <winrt/base.h>
#include <memory>
#include "graphics.h"
#include "camera.h"

class GraphicsTest
{
public:
	GraphicsTest(std::shared_ptr<Graphics> pGraphics) : m_pGraphics{ pGraphics } {}
	GraphicsTest(const GraphicsTest&) = delete;
	GraphicsTest(GraphicsTest&&) = delete;
	GraphicsTest& operator=(const GraphicsTest&) = delete;
	GraphicsTest& operator=(GraphicsTest&&) = delete;
	~GraphicsTest() = default;

	[[nodiscard]] bool load();
	void draw(const Camera & camera) const;

protected:
	std::shared_ptr<Graphics> m_pGraphics{ nullptr };
	winrt::com_ptr<ID3D11Buffer> m_pVertexBuffer{ nullptr };
	winrt::com_ptr<ID3D11Buffer> m_pIndexBuffer{ nullptr };
	winrt::com_ptr<ID3D11Buffer> m_pConstantBuffer{ nullptr };
	winrt::com_ptr<ID3D11VertexShader> m_pVertexShader{ nullptr };
	winrt::com_ptr<ID3D11InputLayout> m_pInputLayout{ nullptr };
	winrt::com_ptr<ID3D11PixelShader> m_pPixelShader{ nullptr };
	winrt::com_ptr<ID3D11SamplerState> m_pSamplerState{ nullptr };
	winrt::com_ptr<ID3D11Texture2D> m_pTexture{ nullptr };
	winrt::com_ptr<ID3D11ShaderResourceView> m_pTextureView{ nullptr };
};
