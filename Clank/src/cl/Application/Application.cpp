#include "cl/stdafx.h"
#include "Application.h"

#include <Windowsx.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace cl {

	LRESULT CALLBACK WndProcBind(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return Application::Get().WndProc(hWnd, message, wParam, lParam);
	}
	
	Application* Application::s_pInstance = NULLPTR;

	Application::Application(String name, ApplicationSettings settings)
		: m_sName(std::move(name)), m_AppSettings(std::move(settings)), m_bClosed(FALSE)
	{
		s_pInstance = this;
	}

	void Application::Start()
	{
		RegisterWindow();
		DoWindow();

		while (true)
		{
			DoWindowMessages();
			Sleep(1);
		}
	}

	void Application::DoWindowMessages()
	{
		MSG message;
		while (PeekMessage(&message, NULL, NULL, NULL, PM_REMOVE) > 0)
		{
			if (message.message == WM_QUIT)
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