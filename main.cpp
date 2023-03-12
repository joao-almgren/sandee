#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <memory>
#include "graphics.h"
#include "camera.h"
#include "graphicstest.h"

int WINAPI wWinMain(
	_In_ const HINSTANCE hInstance,
	_In_opt_ const HINSTANCE /*hPrevInstance*/,
	_In_ const LPWSTR /*lpCmdLine*/,
	_In_ const int /*nShowCmd*/)
{
	const auto windowTitle = L"D3D11Test";

	const WNDCLASSEX wc
	{
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc{ [](const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam) -> LRESULT
		{
			switch (message)
			{
			case WM_KEYDOWN:
				switch (wParam)  // NOLINT(hicpp-multiway-paths-covered)
				{
				case VK_ESCAPE:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					return 0;
				default: ;
				}
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			default: ;
			}

			return DefWindowProc(hWnd, message, wParam, lParam);
		}},
		.hInstance = hInstance,
		.lpszClassName = windowTitle,
	};

	if (!RegisterClassExW(&wc))
		return 0;

	const int windowWidth = 800;
	const int windowHeight = 600;

	RECT windowRect{ .right = windowWidth, .bottom = windowHeight };
	AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);

	const auto adjustedWindowWidth = windowRect.right - windowRect.left;
	const auto adjustedWindowHeight = windowRect.bottom - windowRect.top;
	const auto windowOriginLeft = (GetSystemMetrics(SM_CXSCREEN) - adjustedWindowWidth) / 2;
	const auto windowOriginTop = (GetSystemMetrics(SM_CYSCREEN) - adjustedWindowHeight) / 2;

	const auto hWnd = CreateWindowExW
	(
		WS_EX_OVERLAPPEDWINDOW,
		windowTitle,
		windowTitle,
		WS_OVERLAPPEDWINDOW,
		windowOriginLeft,
		windowOriginTop,
		adjustedWindowWidth,
		adjustedWindowHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hWnd)
		return 0;

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	SetFocus(hWnd);
	ShowCursor(FALSE);

	Graphics graphics;
	if (!graphics.initialize(hWnd, windowWidth, windowHeight))
		return 0;

	GraphicsTest graphicsTest(std::make_shared<Graphics>(graphics));
	if (!graphicsTest.load())
		return 0;

	Camera camera;
	camera.setProjection(3.14f / 2, windowWidth / static_cast<float>(windowHeight), 0.1f, 1000.0f);
	camera.moveForward(-3);

	MSG msg{};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			camera.resetView();

			graphics.resetRenderTarget();
			graphics.clearScreen({ 0.5, 0, 0.5, 0 });

			graphicsTest.draw(camera);

			graphics.present();
		}
	}

	return 0;
}
