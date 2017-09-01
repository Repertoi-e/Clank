#include "cl/stdafx.h"
#include "Application.h"

#include "cl/Maths/maths.h"
#include "cl/Graphics/Context.h"
#include "cl/Graphics/Layers/Layer.h"

#include <Windowsx.h>
#include <shellapi.h>

#include <FreeImage.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace cl {

	Application& g_Application = Application::Instance();
	ApplicationDesc& g_ApplicationDesc = Application::Instance().GetDesc();

	LRESULT CALLBACK WndProcBind(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return g_Application.WndProc(hWnd, message, wParam, lParam);
	}

	Application::Application(void)
	{
	}

	void Application::Create(const ApplicationDesc& appDesc)
	{
		m_Desc = appDesc;
		DoWindow();

		Context::Instance().Create(m_Desc.HWnd);

		FreeImage_Initialise();

		wchar file[1024] = { 0 };
		GetModuleFileNameW(NULL, file, 1024);

		String& path = m_Desc.Path;
		path = String(file);
		path = path.substr(0, path.find_last_of(L'\\') + 1);

		m_Desc.Args = CommandLineToArgvW(GetCommandLineW(), &m_Desc.ArgsCount);

		m_Desc.EntryPoint(path, m_Desc.Args, m_Desc.ArgsCount);
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
		while (!m_Desc.Closed)
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
			if (m_Desc.LimitCycle || !m_Desc.WindowFocused)
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
				m_Desc.Closed = true;
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
			m_Desc.WindowFocused = e.Focused();
			return FALSE;
		});

		for (u32 i = 0; i < m_Layers->Size(); i++)
		{
			Layer* layer = *cast(Layer**) m_Layers->Get(i);
			layer->OnEvent(event);
		}
	}

	void Application::DoRender(void)
	{
		for (u32 i = 0; i < m_Layers->Size(); i++)
		{
			Layer* layer = *cast(Layer**) m_Layers->Get(i);
			layer->OnRender();
		}
	}

	void Application::DoUpdate(const DeltaTime& dt)
	{
		for (u32 i = 0; i < m_Layers->Size(); i++)
		{
			Layer* layer = *cast(Layer**) m_Layers->Get(i);
			layer->OnUpdate(dt);
		}
	}

	void Application::DoTick(void)
	{
		for (u32 i = 0; i < m_Layers->Size(); i++)
		{
			Layer* layer = *cast(Layer**) m_Layers->Get(i);
			layer->OnTick();
		}
	}

	Layer* Application::PushLayer(Layer* layer)
	{
		m_Layers->PushBack(&layer);
		layer->OnInit();

		return layer;
	}

	void Application::PopLayer(Layer* layer)
	{
		for (u32 i = 0; i < m_Layers->Size(); i++)
		{
			if (*cast(Layer**) m_Layers->Get(i) == layer)
			{
				m_Layers->Erase(i);
				del layer;
			}
		}
	}

	void Application::DoWindow(void)
	{
		m_Desc.HInstance = (HINSTANCE)&__ImageBase;;

		WNDCLASSEX wcex;
		ZeroMemory(&wcex, sizeof(WNDCLASSEX));
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.hInstance = m_Desc.HInstance;
		wcex.lpfnWndProc = WndProcBind;
		if (m_Desc.Icon != -1)
			wcex.hIcon = LoadIcon(m_Desc.HInstance, MAKEINTRESOURCE(m_Desc.Icon));
		else
			wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = m_Desc.ClassName.c_str();
		wcex.lpszMenuName = NULL;
		if (m_Desc.SmallIcon != -1)
			wcex.hIconSm = LoadIcon(m_Desc.HInstance, MAKEINTRESOURCE(m_Desc.SmallIcon));
		else
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

		m_Desc.HWnd = CreateWindow(m_Desc.ClassName.c_str(), m_Desc.Name.c_str(), m_Desc.WindowStyle,
			GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2,
			GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2, width, height, NULL, NULL, m_Desc.HInstance, NULL);
		if (!m_Desc.HWnd)
		{
			MessageBox(NULL, L"Failed to create window", NULL, NULL);
			return;
		}

		if (m_Desc.AcceptDroppedFiles)
			DragAcceptFiles(m_Desc.HWnd, TRUE);
	}

	void Application::ShowWindow(void)
	{
		::ShowWindow(m_Desc.HWnd, SW_SHOW);
	}

	void Application::SetWindowTitle(const String& title)
	{
		SetWindowText(m_Desc.HWnd, title.c_str());
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
			if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
			{
				WORD width = LOWORD(lParam);
				WORD height = HIWORD(lParam);

				m_Desc.Width = width;
				m_Desc.Height = height;

				DoEvent(WindowResizeEvent(width, height));

				Context::Instance().ChangeSize(m_Desc.Width, m_Desc.Height);
			}
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
			ClientToScreen(m_Desc.HWnd, &pt);

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
			ClientToScreen(m_Desc.HWnd, &pt);

			DoEvent(MouseClickedEvent(pt.x, pt.y, MOUSE_LEFT, GET_MODS));
		}
		break;
		case WM_LBUTTONUP:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToScreen(m_Desc.HWnd, &pt);

			DoEvent(MouseReleasedEvent(pt.x, pt.y, MOUSE_LEFT, GET_MODS));
		}
		break;
		case WM_RBUTTONDOWN:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToScreen(m_Desc.HWnd, &pt);

			DoEvent(MouseClickedEvent(pt.x, pt.y, MOUSE_RIGHT, GET_MODS));
		}
		break;
		case WM_MOUSEWHEEL:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToScreen(m_Desc.HWnd, &pt);

			DoEvent(MouseScrollEvent(pt.x, pt.y, GET_WHEEL_DELTA_WPARAM(wParam)));
		}
		break;
		case WM_RBUTTONUP:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToScreen(m_Desc.HWnd, &pt);

			DoEvent(MouseReleasedEvent(pt.x, pt.y, MOUSE_RIGHT, GET_MODS));
		}
		case WM_DROPFILES:
		{
			wchar filePaths[MAX_PATH] = { 0 };
			u32 files = 0;
			HDROP drop = (HDROP)wParam;

			files = DragQueryFile(drop, 0xFFFFFFFF, NULL, NULL);
			filePaths[0] = '\0';
			if (DragQueryFile(drop, 0, filePaths, MAX_PATH))
				DoEvent(WindowFileDropEvent(String(filePaths)));

			DragFinish(drop);
			break;
		}
		break;
		default:
			result = DefWindowProc(hWnd, message, wParam, lParam);
		}
		return result;
	}
}