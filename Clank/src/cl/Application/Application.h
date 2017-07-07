#pragma once

#include "cl/stdafx.h"

#include "Events/Events.h"

#include <Windows.h>

namespace cl {

	struct API ApplicationSettings
	{
		WORD WIDTH, HEIGHT;
		BOOL VSYNC, FULLSCREEN;
		DWORD WINDOW_STYLE;
	};

	class API Application
	{
		static Application* s_pInstance;
	private:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		
		String m_sName;
		ApplicationSettings m_AppSettings;

		BOOL m_bClosed;
	public:
		Application(String name, ApplicationSettings settings);

		void Start();

		void DoWindowMessages();

		void DoEvent(Event& event);

		inline String& GetName() { return m_sName; }
		inline void GetSettings(ApplicationSettings* settings) { *settings = m_AppSettings; }
		inline void SetSettings(ApplicationSettings* settings) { m_AppSettings = *settings; }

		void SetWindowTitle(LPCWSTR title);

		LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		void RegisterWindow();
		void DoWindow();
	public:
		inline static Application& Get() { return *s_pInstance; }
	};
}