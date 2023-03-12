#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <winrt/base.h>
#include <memory>
#include "graphics.h"

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
	void draw(winrt::com_ptr<ID3D11DeviceContext> pDeviceContext, const DirectX::XMMATRIX & view, const DirectX::XMMATRIX & proj) const;

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
