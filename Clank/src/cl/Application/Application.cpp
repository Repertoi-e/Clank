#include "cl/stdafx.h"
#include "Application.h"

#include "cl/Maths/maths.h"

#include <Windowsx.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace cl {

	Application& g_Application = Application::Instance();

	LRESULT CALLBACK WndProcBind(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return g_Application.WndProc(hWnd, message, wParam, lParam);
	}

	Application::Application()
		: m_bClosed(FALSE)
	{
	}

	struct FPSInfo
	{
		static const s32 MaxSamples = 64;

		s32 It;
		float32 FpsSamples[MaxSamples] = { 0.0f };
	} g_FpsInfo;

	void Application::DoFPS(CycleInfo& info)
	{
		g_FpsInfo.FpsSamples[g_FpsInfo.It % g_FpsInfo.MaxSamples] = float32(info.Frames * m_UpdatesPerSecond);
		for (s32 i = 0; i < g_FpsInfo.MaxSamples; i++)
			m_FramesPerSecond += g_FpsInfo.FpsSamples[i];
		m_FramesPerSecond /= g_FpsInfo.MaxSamples;
		g_FpsInfo.It = g_FpsInfo.It == g_FpsInfo.MaxSamples + 1 ? 0 : g_FpsInfo.It + 1;
		info.Frames = 0;
	}

	vec4 blue = { 0.0f, 0.2f, 0.4f, 1.0f };
	float32 elapsed = 0, colort;

	void Application::DoCycle()
	{
		m_CycleInfo.Timer = new Timer();
		m_CycleInfo.UpdateTimer = m_CycleInfo.Timer->Elapsed().Millis();
		m_CycleInfo.UpdateDeltaTime = new DeltaTime(m_CycleInfo.Timer->Elapsed().Millis());
		while (!m_bClosed)
		{
			float32 now = m_CycleInfo.Timer->Elapsed().Millis();
			if (now - m_CycleInfo.UpdateTimer > m_CycleInfo.UpdateTick)
			{
				m_CycleInfo.UpdateDeltaTime->Update(now);
				{
					DoWindowMessages();
				
					elapsed++;
					colort = (sin(elapsed / 10) + 1) / 2;
				}
				m_CycleInfo.Updates++;
				m_CycleInfo.UpdateTimer += m_CycleInfo.UpdateTick;
				
				DoFPS(m_CycleInfo);

				SetWindowTitle(String(m_sName + L" | FPS: " + std::to_wstring(m_FramesPerSecond)).c_str());
			}
			{
				Timer frametimer;

				float32 fcolor[4] = { blue.r * colort, blue.g * colort, blue.b * colort, blue.a };
				m_pContext->GetDeviceContext()->ClearRenderTargetView(m_pContext->GetBackbuffer(), fcolor);

				m_CycleInfo.Frames++;
				m_Frametime = frametimer.Elapsed().Millis();

				m_pContext->GetSwapChain()->Present(0, 0);
			}
			if (m_CycleInfo.Timer->Elapsed().Seconds() - m_CycleInfo.ElapsedSeconds > 1.f)
			{
				m_CycleInfo.ElapsedSeconds += 1.f;
				m_UpdatesPerSecond = m_CycleInfo.Updates;
				std::cout << m_CycleInfo.Updates << std::endl;
				m_CycleInfo.Updates = 0;
			}
			if (!m_bWindowFocused)
				Sleep(5);
		}
	}

	void Application::DoWindowMessages()
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
	}

	void Application::RegisterWindow()
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
	}

	void Application::DoWindow()
	{
		RECT r = { 0, 0, m_AppSettings.WIDTH, m_AppSettings.HEIGHT };
		AdjustWindowRect(&r, m_AppSettings.WINDOW_STYLE, false);
		s32 width = r.right - r.left;
		s32 height = r.bottom - r.top;

		m_hWnd = CreateWindow(L"Clank Engine Application Window", L"Clank Window", m_AppSettings.WINDOW_STYLE,
			GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2,
			GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2, width, height, NULL, NULL, m_hInstance, NULL);
		if (!m_hWnd)
		{
			MessageBox(NULL, L"Failed to create window", NULL, NULL);
			return;
		}

		ShowWindow(m_hWnd, SW_SHOW);
	}

	void Application::DoD3DContext()
	{
		m_pContext = new Context();
		m_pContext->Create(m_hWnd);
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

			m_AppSettings.WIDTH = width;
			m_AppSettings.HEIGHT = height;

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