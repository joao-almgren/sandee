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
	void draw(winrt::com_ptr<ID3D11DeviceContext> pDeviceContext) const;

protected:
	winrt::com_ptr<ID3D11Buffer> pVertexBuffer;
	winrt::com_ptr<ID3D11Buffer> pIndexBuffer;
	winrt::com_ptr<ID3D11VertexShader> pVertexShader;
	winrt::com_ptr<ID3D11PixelShader> pPixelShader;
	winrt::com_ptr<ID3D11InputLayout> pInputLayout;
};
