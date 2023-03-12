#pragma once
#include <d3d11.h>
#include <winrt/base.h>
#include <memory>
#include "graphics.h"
#include "camera.h"

class GraphicsTest
{
public:
	GraphicsTest(std::shared_ptr<Graphics> pGraphics) : pGraphics{ pGraphics } {}
	GraphicsTest(const GraphicsTest& g) = delete;
	GraphicsTest(GraphicsTest&& g) = delete;
	GraphicsTest& operator=(const GraphicsTest& g) = delete;
	GraphicsTest& operator=(GraphicsTest&& g) = delete;
	~GraphicsTest() = default;

	[[nodiscard]] bool load(winrt::com_ptr<ID3D11Device> pDevice);
	void draw(const Camera & camera) const;

protected:
	std::shared_ptr<Graphics> pGraphics{ nullptr };
	winrt::com_ptr<ID3D11Buffer> pVertexBuffer{ nullptr };
	winrt::com_ptr<ID3D11Buffer> pIndexBuffer{ nullptr };
	winrt::com_ptr<ID3D11Buffer> pConstantBuffer{ nullptr };
	winrt::com_ptr<ID3D11VertexShader> pVertexShader{ nullptr };
	winrt::com_ptr<ID3D11InputLayout> pInputLayout{ nullptr };
	winrt::com_ptr<ID3D11PixelShader> pPixelShader{ nullptr };
	winrt::com_ptr<ID3D11SamplerState> pSamplerState{ nullptr };
	winrt::com_ptr<ID3D11Texture2D> pTexture{ nullptr };
	winrt::com_ptr<ID3D11ShaderResourceView> pTextureView{ nullptr };
};
