#include "cl/stdafx.h"
#include "Application.h"

#include "cl/Maths/maths.h"
#include "cl/Graphics/Context.h"
#include "cl/Graphics/Layers/Layer.h"

#include <Windowsx.h>

#include <FreeImage.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace cl {

	Application& g_Application = Application::Instance();

	LRESULT CALLBACK WndProcBind(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return g_Application.WndProc(hWnd, message, wParam, lParam);
	}

	Application::Application(void)
	{
	}

	void Application::Create(ApplicationDesc& appDesc)
	{
		SetDescription(appDesc);
		DoWindow();
		
		Context::Instance().Create(m_hWnd, m_Desc);

		FreeImage_Initialise();

		wchar file[1024 * 10] = { 0 };
		GetModuleFileNameW(NULL, file, 1024);

		String& path = m_Desc.Path;
		path = String(file);
		path = path.substr(0, path.find_last_of(L'\\') + 1);
	}

	void Application::DoFPS(void)
	{
		CycleDesc& info = m_Desc.Cycle;
		info.FpsSamples[info.It % info.MaxSamples] = float32(info.Frames * info.UPS);
		for (s32 i = 0; i < info.MaxSamples; i++)
			info.FPS += info.FpsSamples[i];
		info.FPS /= info.MaxSamples;
		info.It = info.It == info.MaxSamples + 1 ? 0 : info.It + 1;
		info.Frames = 0;
	}

	void Application::Start(void)
	{
		CycleDesc& info = m_Desc.Cycle;
		while (!m_Closed)
		{
			float32 now = info.Timer->Elapsed().Millis();
			if (now - info.UpdateTimer > info.UpdateTick)
			{
				info.UpdateDeltaTime->Update(now);
				DoWindowMessages();

				DoUpdate(*info.UpdateDeltaTime);

				info.Updates++;
				info.UpdateTimer += info.UpdateTick;

				DoFPS();
			}
			{
				Context::Instance().Clear(vec4(1, 1, 1, 1));

				Timer frametimer;
				{
					DoRender();
				}
				info.FrameTime = frametimer.Elapsed().Millis();
				info.Frames++;

				Context::Instance().Present();
			}
			if (info.Timer->Elapsed().Seconds() - info.ElapsedSeconds > 1.f)
			{
				info.ElapsedSeconds += 1.f;
				info.UPS = info.Updates;
				DoTick();
				info.Updates = 0;
			}
			if (!m_WindowFocused)
				Sleep(5);
		}
	}

	void Application::DoWindowMessages(void)
	{
		MSG message;
		while (PeekMessage(&message, NULL, NULL, NULL, PM_REMOVE) > 0)
		{
			if (WM_QUIT == message.message)
			{
				m_Closed = true;
				return;
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	void Application::DoEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowFocusEvent>([&](WindowFocusEvent& e) -> BOOL
		{
			m_WindowFocused = e.Focused();
			return FALSE;
		});

		for (auto a : m_Layers)
			a->OnEvent(event);
	}

	void Application::DoRender(void)
	{
		for (auto a : m_Layers)
			a->OnRender();
	}

	void Application::DoUpdate(const DeltaTime& dt)
	{
		for (auto a : m_Layers)
			a->OnUpdate(dt);
	}

	void Application::DoTick(void)
	{
		for (auto a : m_Layers)
			a->OnTick();
	}

	Layer* Application::PushLayer(Layer* layer)
	{
		m_Layers.push_back(layer);
		layer->OnInit();

		return layer;
	}

	void Application::PopLayer(Layer* layer)
	{
		for (u32 i = 0; i < m_Layers.size(); i++)
			if (m_Layers[i] == layer)
			{
				m_Layers.erase(m_Layers.begin() + i);
				del layer;
			}
	}

	void Application::DoWindow(void)
	{
		m_hInstance = (HINSTANCE)&__ImageBase;;

		WNDCLASSEX wcex;
		ZeroMemory(&wcex, sizeof(WNDCLASSEX));
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.hInstance = m_hInstance;
		wcex.lpfnWndProc = WndProcBind;
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = m_Desc.ClassName.c_str();
		wcex.lpszMenuName = NULL;
		wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		if (!RegisterClassEx(&wcex))
		{
			MessageBox(NULL, L"Failed to register window", NULL, NULL);
			return;
		}

		RECT r = { 0, 0, cast(LONG) m_Desc.Width, cast(LONG) m_Desc.Height };
		AdjustWindowRect(&r, m_Desc.WindowStyle, false);
		s32 width = r.right - r.left;
		s32 height = r.bottom - r.top;

		m_hWnd = CreateWindow(m_Desc.ClassName.c_str(), m_Desc.Name.c_str(), m_Desc.WindowStyle,
			GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2,
			GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2, width, height, NULL, NULL, m_hInstance, NULL);
		if (!m_hWnd)
		{
			MessageBox(NULL, L"Failed to create window", NULL, NULL);
			return;
		}
	}

	void Application::ShowWindow(void)
	{
		::ShowWindow(m_hWnd, SW_SHOW);
	}

	void Application::SetWindowTitle(const String& title)
	{
		SetWindowText(m_hWnd, title.c_str());
	}

	LRESULT CALLBACK Application::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = NULL;

		switch (message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_SIZE:
		{
			WORD width = LOWORD(lParam);
			WORD height = HIWORD(lParam);

			m_Desc.Width = width;
			m_Desc.Height = height;

			DoEvent(WindowResizeEvent(width, height));
		}
		break;
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
				DoEvent(WindowFocusEvent(false));
			else
				DoEvent(WindowFocusEvent(true));
			break;
		case WM_MOUSEMOVE:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToScreen(m_hWnd, &pt);

			DoEvent(MouseMovedEvent(pt.x, pt.y, GetKeyState(VK_LBUTTON) < 0));
		}
		break;
		case WM_KEYDOWN:
			DoEvent(KeyPressedEvent(wParam, GET_MODS, (lParam >> 30) & 1));
			break;
		case WM_KEYUP:
			DoEvent(KeyReleasedEvent(wParam, GET_MODS));
			break;
		case WM_LBUTTONDOWN:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToScreen(m_hWnd, &pt);

			DoEvent(MouseClickedEvent(pt.x, pt.y, MOUSE_LEFT, GET_MODS));
		}
		break;
		case WM_LBUTTONUP:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToScreen(m_hWnd, &pt);

			DoEvent(MouseReleasedEvent(pt.x, pt.y, MOUSE_LEFT, GET_MODS));
		}
		break;
		case WM_RBUTTONDOWN:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToScreen(m_hWnd, &pt);

			DoEvent(MouseClickedEvent(pt.x, pt.y, MOUSE_RIGHT, GET_MODS));
		}
		break;
		case WM_MOUSEWHEEL:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToScreen(m_hWnd, &pt);

			DoEvent(MouseScrollEvent(pt.x, pt.y, GET_WHEEL_DELTA_WPARAM(wParam)));
		}
		break;
		case WM_RBUTTONUP:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToScreen(m_hWnd, &pt);

			DoEvent(MouseReleasedEvent(pt.x, pt.y, MOUSE_RIGHT, GET_MODS));
		}
		break;
		default:
			result = DefWindowProc(hWnd, message, wParam, lParam);
		}
		return result;
	}
}