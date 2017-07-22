#pragma once

#include "cl/stdafx.h"

#include "cl/Utils/Singleton.h"
#include "cl/Utils/Time.h"

#include "Events/Events.h"

#include <Windows.h>

namespace cl {

	class Layer;

    struct API ApplicationSettings
    {
        WORD Width, Height;
        BOOL VSync, Fullscreen;
        DWORD WindowStyle;
    };

	struct API CycleInfo
	{
		Timer* Timer;
		DeltaTime* UpdateDeltaTime;
		float32 ElapsedSeconds;
		float32 UpdateTimer;
		float32 UpdateTick;
		u32 Frames, Updates;
		s32 m_UpdatesPerSecond;
		float32 m_FramesPerSecond, m_Frametime;
	};

    class API Application : public Singleton<Application>
	{
    private:
        HINSTANCE m_hInstance;
        HWND m_hWnd;

		BOOL m_bWindowFocused;

        String m_sName;
        ApplicationSettings m_AppSettings;
		CycleInfo m_CycleInfo;

        BOOL m_bClosed;

		std::vector<Layer*> m_Layers;
    public:
        Application(void);

		void DoWindow(void);
		void ShowWindow(void);
		void DoCycle(void);

        void DoWindowMessages(void);
        void DoEvent(Event& event);
		void DoRender(void);
		void DoUpdate(const DeltaTime& dt);
		void DoTick(void);

		Layer* PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

        inline const String& GetName() const { return m_sName; }
		inline void SetName(const String& name) { m_sName = name; }

		inline ApplicationSettings& GetSettings() { return m_AppSettings; }
		inline void SetSettings(ApplicationSettings settings) { m_AppSettings = std::move(settings); }
		
		inline CycleInfo& GetCycleInfo() { return m_CycleInfo; }
		inline void SetCycleInfo(CycleInfo info) { m_CycleInfo = std::move(info); }

        void SetWindowTitle(LPCWSTR title);

        LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		void DoFPS();
    };

    extern API Application& g_Application;
}