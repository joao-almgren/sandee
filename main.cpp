#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "graphics.h"
#include "graphicstest.h"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

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

	if (!RegisterClassEx(&wc))
		return 0;

	RECT windowRect{ .right = WINDOW_WIDTH, .bottom = WINDOW_HEIGHT };
	AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);

	const auto windowWidth = windowRect.right - windowRect.left;
	const auto windowHeight = windowRect.bottom - windowRect.top;
	const auto windowLeft = (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2;
	const auto windowTop = (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2;

	const auto hWnd{ CreateWindowEx
	(
		WS_EX_OVERLAPPEDWINDOW,
		windowTitle,
		windowTitle,
		WS_OVERLAPPEDWINDOW,
		windowLeft,
		windowTop,
		windowWidth,
		windowHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	)};

	if (!hWnd)
		return 0;

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	SetFocus(hWnd);
	ShowCursor(FALSE);

	Graphics graphics;
	if (!graphics.initialize(hWnd, WINDOW_WIDTH, WINDOW_HEIGHT))
		return 0;

	GraphicsTest graphicsTest;
	if (!graphicsTest.load(graphics.getDevice()))
		return 0;

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
			graphics.resetRenderTarget();
			graphics.clearScreen({ 0.5, 0, 0.5, 0 });
			graphicsTest.draw(graphics.getDeviceContext());
			graphics.present();
		}
	}

	return 0;
}
