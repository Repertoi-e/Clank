#pragma once

#include "cl/stdafx.h"

#include "cl/Graphics/Context.h"
#include "cl/Utils/Singleton.h"
#include "cl/Utils/Time.h"

#include "Events/Events.h"

#include <Windows.h>

namespace cl {

    struct API ApplicationSettings
    {
        WORD WIDTH, HEIGHT;
        BOOL VSYNC, FULLSCREEN;
        DWORD WINDOW_STYLE;
    };

    class API Application : public Singleton<Application>
	{
    private:
        HINSTANCE m_hInstance;
        HWND m_hWnd;

		BOOL m_bWindowFocused;
        
        String m_sName;
        ApplicationSettings m_AppSettings;

		Timer* m_pLoopTimer;
        BOOL m_bClosed;

		Context* m_pContext;
    public:
        Application();

		void RegisterWindow();
		void DoWindow();

		void DoD3DContext();

		void Start();

        void DoWindowMessages();

        void DoEvent(Event& event);

        inline String& GetName() { return m_sName; }
        inline void GetSettings(ApplicationSettings* settings) { *settings = m_AppSettings; }
        inline void SetSettings(ApplicationSettings* settings) { m_AppSettings = *settings; }

        void SetWindowTitle(LPCWSTR title);

        LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    };

    extern API Application& g_Application;
}