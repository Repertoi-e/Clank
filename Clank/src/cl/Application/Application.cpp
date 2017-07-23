#include "cl/stdafx.h"
#include "Application.h"

#include "cl/Maths/maths.h"
#include "cl/Graphics/Context.h"
#include "cl/Graphics/Layers/Layer.h"

#include <Windowsx.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace cl {

	Application& g_Application = Application::Instance();

	LRESULT CALLBACK WndProcBind(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return g_Application.WndProc(hWnd, message, wParam, lParam);
	}

	Application::Application(void)
		: m_bClosed(FALSE)
	{
	}

	void Application::DoFPS(void)
	{
		FpsSamples[It % MaxSamples] = float32(m_CycleInfo.Frames * m_CycleInfo.m_UpdatesPerSecond);
		for (s32 i = 0; i < MaxSamples; i++)
			m_CycleInfo.m_FramesPerSecond += FpsSamples[i];
		m_CycleInfo.m_FramesPerSecond /= MaxSamples;
		It = It == MaxSamples + 1 ? 0 : It + 1;
		m_CycleInfo.Frames = 0;
	}

	void Application::DoCycle(void)
	{
		m_CycleInfo.Timer = new Timer;
		m_CycleInfo.UpdateTimer = m_CycleInfo.Timer->Elapsed().Millis();
		m_CycleInfo.UpdateDeltaTime = new DeltaTime(m_CycleInfo.Timer->Elapsed().Millis());
		while (!m_bClosed)
		{
			float32 now = m_CycleInfo.Timer->Elapsed().Millis();
			if (now - m_CycleInfo.UpdateTimer > m_CycleInfo.UpdateTick)
			{
				m_CycleInfo.UpdateDeltaTime->Update(now);
				DoWindowMessages();

				DoUpdate(*m_CycleInfo.UpdateDeltaTime);

				m_CycleInfo.Updates++;
				m_CycleInfo.UpdateTimer += m_CycleInfo.UpdateTick;

				DoFPS();

				SetWindowTitle(String(m_sName + L" | FPS: " + std::to_wstring(m_CycleInfo.m_FramesPerSecond)).c_str());
			}
			{
				Context::Instance().Clear(vec4(1, 1, 1, 1));

				Timer frametimer;
				{
					DoRender();
				}
				m_CycleInfo.m_Frametime = frametimer.Elapsed().Millis();
				m_CycleInfo.Frames++;

				Context::Instance().Present();
			}
			if (m_CycleInfo.Timer->Elapsed().Seconds() - m_CycleInfo.ElapsedSeconds > 1.f)
			{
				m_CycleInfo.ElapsedSeconds += 1.f;
				m_CycleInfo.m_UpdatesPerSecond = m_CycleInfo.Updates;
				DoTick();
				m_CycleInfo.Updates = 0;
			}
			if (!m_bWindowFocused)
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
				m_bClosed = true;
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
			m_bWindowFocused = e.Focused();
			return FALSE;
		});

		for (auto a : m_Layers)
			a->Event(event);
	}

	void Application::DoRender(void)
	{
		for (auto a : m_Layers)
			a->Render();
	}

	void Application::DoUpdate(const DeltaTime& dt)
	{
		for (auto a : m_Layers)
			a->Update(dt);
	}

	void Application::DoTick(void)
	{
		for (auto a : m_Layers)
			a->Tick();
	}

	Layer* Application::PushLayer(Layer* layer)
	{
		m_Layers.push_back(layer);
		layer->Init(&Context::Instance());

		return layer;
	}

	void Application::PopLayer(Layer* layer)
	{
		for (u32 i = 0; i < m_Layers.size(); i++)
			if (m_Layers[i] == layer)
			{
				m_Layers.erase(m_Layers.begin() + i);
				delete layer;
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
		wcex.lpszClassName = L"Clank Engine Application Window";
		wcex.lpszMenuName = NULL;
		wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		if (!RegisterClassEx(&wcex))
		{
			MessageBox(NULL, L"Failed to register window", NULL, NULL);
			return;
		}

		RECT r = { 0, 0, m_AppSettings.Width, m_AppSettings.Height };
		AdjustWindowRect(&r, m_AppSettings.WindowStyle, false);
		s32 width = r.right - r.left;
		s32 height = r.bottom - r.top;

		m_hWnd = CreateWindow(L"Clank Engine Application Window", L"Clank Window", m_AppSettings.WindowStyle,
			GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2,
			GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2, width, height, NULL, NULL, m_hInstance, NULL);
		if (!m_hWnd)
		{
			MessageBox(NULL, L"Failed to create window", NULL, NULL);
			return;
		}

		Context::Instance().Create(m_hWnd, m_AppSettings);
	}

	void Application::ShowWindow(void)
	{
		::ShowWindow(m_hWnd, SW_SHOW);
	}

	void Application::SetWindowTitle(LPCWSTR title)
	{
		SetWindowText(m_hWnd, title);
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

			m_AppSettings.Width = width;
			m_AppSettings.Height = height;

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