#pragma once
#include <d3d11.h>
#include "winrt/base.h"

class GraphicsTest
{
public:
	GraphicsTest() = default;
	GraphicsTest(const GraphicsTest& g) = delete;
	GraphicsTest(GraphicsTest&& g) = delete;
	GraphicsTest& operator=(const GraphicsTest& g) = delete;
	GraphicsTest& operator=(GraphicsTest&& g) = delete;
	~GraphicsTest() = default;

	[[nodiscard]] bool load(winrt::com_ptr<ID3D11Device> pDevice);
	void draw(winrt::com_ptr<ID3D11DeviceContext> pDeviceContext, int windowWidth, int windowHeight) const;

protected:
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
