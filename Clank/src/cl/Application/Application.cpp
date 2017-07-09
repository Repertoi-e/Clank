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

#define MAXSAMPLES 100
	int tickindex = 0;
	int ticksum = 0;
	int ticklist[MAXSAMPLES];

	/* need to zero out the ticklist array before starting */
	/* average will ramp up until the buffer is full */
	/* returns average ticks per frame over the MAXSAMPLES last frames */

	double CalcAverageTick(int newtick)
	{
		ticksum -= ticklist[tickindex];  /* subtract value falling off */
		ticksum += newtick;              /* add new value */
		ticklist[tickindex] = newtick;   /* save new value so it can be subtracted later */
		if (++tickindex == MAXSAMPLES)    /* inc buffer index */
			tickindex = 0;

		/* return average */
		return((double)ticksum / MAXSAMPLES);
	}

	vec4 blue = { 0.0f, 0.2f, 0.4f, 1.0f };
	float32 elapsed = 0, colort;

	void Application::DoCycle()
	{
		m_pLoopTimer = new Timer();
		float32 timer = 0.0f;
		float32 updateTimer = m_pLoopTimer->Elapsed().Millis();
		float32 updateTick = 1000.f / 60.f;
		u32 frames = 0;
		s32 curr = 0;
		const s32 fpsSamplesMax = 200;
		float32 fpsSamples[fpsSamplesMax] = { 0 };
		u32 updates = 0;
		DeltaTime delta(m_pLoopTimer->Elapsed().Millis());
		while (!m_bClosed)
		{
			// m_Window->Clear();
			float32 now = m_pLoopTimer->Elapsed().Millis();
			if (now - updateTimer > updateTick)
			{
				delta.Update(now);
				// Update(delta);
				updates++;
				updateTimer += updateTick;

				DoWindowMessages();

				elapsed++;
				colort = (sin(elapsed / 10) + 1) / 2;

				m_FramesPerSecond = 0;
				fpsSamples[curr % fpsSamplesMax] = float32(frames * m_UpdatesPerSecond);
				for (s32 i = 0; i < fpsSamplesMax; i++)
					m_FramesPerSecond += fpsSamples[i];
				m_FramesPerSecond /= fpsSamplesMax;
				curr = curr == fpsSamplesMax + 1 ? 0 : curr + 1;

				frames = 0;

				std::wstringstream wss;
				wss << std::wstring(m_sName.begin(), m_sName.end()) <<" | FPS: " << m_FramesPerSecond;

				SetWindowTitle(wss.str().c_str());
			}
			{
				Timer frametimer;

				float32 fcolor[4] = { blue.r * colort, blue.g * colort, blue.b * colort, blue.a };
				m_pContext->GetDeviceContext()->ClearRenderTargetView(m_pContext->GetBackbuffer(), fcolor);

				frames++;
				m_Frametime = frametimer.Elapsed().Millis();

				m_pContext->GetSwapChain()->Present(0, 0);
			}
			if (m_pLoopTimer->Elapsed().Seconds() - timer > 1.f)
			{
				timer += 1.f;
				m_UpdatesPerSecond = updates;
				std::cout << updates << std::endl;
				updates = 0;
			}
			if (!m_bWindowFocused)
				Sleep(1);
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
		wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
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