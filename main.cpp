#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Keyboard.h>
#include <Mouse.h>
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include <memory>
#include "config.h"
#include "graphics.h"
#include "camera.h"
#include "fps.h"
#include "cube.h"
#include "bunny.h"
#include "skybox.h"
#include "statue.h"

using Mouse = DirectX::Mouse;
using Keyboard = DirectX::Keyboard;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ const HINSTANCE hInstance, _In_opt_ const HINSTANCE /*hPrevInstance*/, _In_ const LPWSTR /*lpCmdLine*/, _In_ const int /*nShowCmd*/)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
		return 0;

	const auto windowTitle = L"sandee";
	const WNDCLASSEX wc
	{
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc{ [](const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam) -> LRESULT
		{
			if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
				return true;

			switch (message) // NOLINT(hicpp-multiway-paths-covered)
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
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				}
			case WM_KEYUP:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
				Keyboard::ProcessMessage(message, wParam, lParam);
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			default:
				return DefWindowProcW(hWnd, message, wParam, lParam);
			}

			return 0;
		}},
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = LoadIcon(hInstance, IDI_APPLICATION),
		.hCursor = LoadCursor(nullptr, IDC_ARROW),
		// ReSharper disable once CppCStyleCast
		.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1), // NOLINT(performance-no-int-to-ptr)
		.lpszMenuName = nullptr,
		.lpszClassName = windowTitle,
		.hIconSm = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION),
	};

	if (!RegisterClassEx(&wc))
		return 0;

	if (!Config::load())
		return 0;

	const int windowWidth = Config::width;
	const int windowHeight = Config::height;
	RECT windowRect{ .right = windowWidth, .bottom = windowHeight };
	AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
	const auto adjustedWindowWidth = windowRect.right - windowRect.left;
	const auto adjustedWindowHeight = windowRect.bottom - windowRect.top;
	const auto windowOriginLeft = (GetSystemMetrics(SM_CXSCREEN) - adjustedWindowWidth) / 2;
	const auto windowOriginTop = (GetSystemMetrics(SM_CYSCREEN) - adjustedWindowHeight) / 2;

	const HWND hWnd = CreateWindowEx
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

	auto keyboard = std::make_unique<Keyboard>();
	Keyboard::KeyboardStateTracker keyboardTracker;
	auto mouse = std::make_unique<Mouse>();
	mouse->SetWindow(hWnd);
	Mouse::ButtonStateTracker mouseTracker;

	Graphics graphics;
	if (!graphics.initialize(hWnd, windowWidth, windowHeight))
		return 0;

	Skybox skybox(std::make_shared<Graphics>(graphics));
	if (!skybox.load())
		return 0;

	Cube cube(std::make_shared<Graphics>(graphics));
	if (!cube.load())
		return 0;

	Statue statue(std::make_shared<Graphics>(graphics));
	if (!statue.load())
		return 0;

	Bunny bunny(std::make_shared<Graphics>(graphics));
	if (!bunny.load())
		return 0;

	Camera camera;
	camera.setProjection(DirectX::XM_PIDIV2, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), Config::nearPlane, Config::farPlane);
	camera.move(0, 0, -8);

	FpsCounter fpsCounter;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(graphics.getDevice().get(), graphics.getDeviceContext().get());

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
			// tick
			{
				fpsCounter.tick();
				const float tick = 60.0f / static_cast<float>(fpsCounter.getAverageFps());

				auto keyboardState = keyboard->GetState();
				keyboardTracker.Update(keyboardState);

				float speed = 0.05f * tick;
				float dX = 0, dY = 0, dZ = 0;
				if (keyboardState.D || keyboardState.Right)
					dX = speed;
				else if (keyboardState.A || keyboardState.Left)
					dX = -speed;
				if (keyboardState.W || keyboardState.Up)
					dZ = speed;
				else if (keyboardState.S || keyboardState.Down)
					dZ = -speed;
				if (keyboardState.Q)
					dY = speed;
				else if (keyboardState.Z)
					dY = -speed;

				camera.move(dX, dY, dZ);

				auto mouseState = mouse->GetState();
				mouseTracker.Update(mouseState);

				if (mouseTracker.rightButton == Mouse::ButtonStateTracker::ButtonState::RELEASED)
				{
					if (mouseState.positionMode == Mouse::MODE_RELATIVE)
						mouse->SetMode(Mouse::MODE_ABSOLUTE);
					else
						mouse->SetMode(Mouse::MODE_RELATIVE);
				}

				if (mouseState.positionMode == Mouse::MODE_RELATIVE)
				{
					camera.rotate(static_cast<float>(-mouseState.y) / 300.0f, static_cast<float>(mouseState.x) / 300.0f);
				}

				skybox.update(tick);
				cube.update(tick);
				statue.update(tick);
				bunny.update(tick);
			}

			// draw
			{
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				ImGui::Begin("FPS");
				ImGui::Text("%.1f fps", fpsCounter.getAverageFps());
				ImGui::End();

				graphics.resetRenderTarget();
				graphics.clearScreen();

				skybox.draw(camera);
				//cube.draw(camera);
				statue.draw(camera);
				//bunny.draw(camera);

				ImGui::EndFrame();
				ImGui::Render();
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

				graphics.present();
			}
		}
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return 0;
}
