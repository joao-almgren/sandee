#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <memory>
#include <Keyboard.h>
#include <Mouse.h>
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include "graphics.h"
#include "camera.h"
#include "graphicstest.h"
#include "fps.h"

using namespace DirectX;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
			if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
				return true;

			switch (message)  // NOLINT(hicpp-multiway-paths-covered)
			{
			case WM_ACTIVATE:
			case WM_ACTIVATEAPP:
				Keyboard::ProcessMessage(message, wParam, lParam);
				Mouse::ProcessMessage(message, wParam, lParam);
				break;
			case WM_INPUT:
			case WM_MOUSEMOVE:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_MOUSEWHEEL:
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
			case WM_MOUSEHOVER:
				Mouse::ProcessMessage(message, wParam, lParam);
				break;
			case WM_KEYDOWN:
				if (wParam == VK_ESCAPE)
				{
					PostMessageW(hWnd, WM_CLOSE, 0, 0);
					return 0;
				}
			case WM_KEYUP:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
				Keyboard::ProcessMessage(message, wParam, lParam);
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			default: ;
			}

			return DefWindowProcW(hWnd, message, wParam, lParam);
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
	//ShowCursor(FALSE);

	auto keyboard = std::make_unique<Keyboard>();
	Keyboard::KeyboardStateTracker keyboardTracker;
	auto mouse = std::make_unique<Mouse>();
	mouse->SetWindow(hWnd);
	Mouse::ButtonStateTracker mouseTracker;

	Graphics graphics;
	if (!graphics.initialize(hWnd, windowWidth, windowHeight))
		return 0;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(graphics.getDevice().get(), graphics.getDeviceContext().get());

	GraphicsTest graphicsTest(std::make_shared<Graphics>(graphics));
	if (!graphicsTest.load())
		return 0;

	Camera camera;
	camera.setProjection(3.14f / 2, windowWidth / static_cast<float>(windowHeight), 0.1f, 1000.0f);
	camera.moveForward(-3);

	FpsCounter fpsCount;

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
			fpsCount.tick();
			const float tick = 60.0f / static_cast<float>(fpsCount.getAverageFps());

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("FPS");
			ImGui::Text("%.1f fps", fpsCount.getAverageFps());
			ImGui::End();

			auto keyboardState = keyboard->GetState();
			keyboardTracker.Update(keyboardState);
			auto mouseState = mouse->GetState();
			mouseTracker.Update(mouseState);

			if (mouseTracker.leftButton == Mouse::ButtonStateTracker::ButtonState::RELEASED)
			{
				if (mouseState.positionMode == Mouse::MODE_RELATIVE)
					mouse->SetMode(Mouse::MODE_ABSOLUTE);
				else
					mouse->SetMode(Mouse::MODE_RELATIVE);
			}

			if (mouseState.positionMode == Mouse::MODE_RELATIVE)
			{
				camera.rotate(static_cast<float>(mouseState.y) / 300.0f, static_cast<float>(mouseState.x) / 300.0f);
			}

			float speed = 0.05f * tick;
			if (keyboardState.D || keyboardState.Right)
				camera.moveRight(speed);
			else if (keyboardState.A || keyboardState.Left)
				camera.moveRight(-speed);
			if (keyboardState.W || keyboardState.Up)
				camera.moveForward(speed);
			else if (keyboardState.S || keyboardState.Down)
				camera.moveForward(-speed);
			if (keyboardState.Q)
				camera.moveUp(speed);
			else if (keyboardState.Z)
				camera.moveUp(-speed);

			camera.resetView();

			graphicsTest.update(tick);

			graphics.resetRenderTarget();
			graphics.clearScreen({ 0.5, 0, 0.5, 0 });

			graphicsTest.draw(camera);

			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			graphics.present();
		}
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return 0;
}
