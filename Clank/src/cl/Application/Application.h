#pragma once

#include "cl/stdafx.h"

#include "cl/Utils/Singleton.h"
#include "cl/Utils/Time.h"

#include "Events/Events.h"

#include <Windows.h>

namespace cl {

	class Layer;

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
	
		// FPS Calculation
		static const s32 MaxSamples = 64;
		s32 It;
		float32 FpsSamples[MaxSamples] = { 0.0f };
	};

    struct API ApplicationSettings
    {
        u32 Width, Height;
        bool VSync, Fullscreen;
        u32 WindowStyle;
		CycleInfo CycleInfo;
    };

    class API Application : public Singleton<Application>
	{
    private:
        HINSTANCE m_hInstance;
        HWND m_hWnd;

		BOOL m_WindowFocused;

        String m_Name;
        ApplicationSettings m_AppSettings;

        BOOL m_Closed;

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

        inline const String& GetName() const { return m_Name; }
		inline void SetName(const String& name) { m_Name = name; }

		inline const ApplicationSettings& GetSettings() { return m_AppSettings; }
		inline void SetSettings(const ApplicationSettings& settings) { m_AppSettings = settings; }

        void SetWindowTitle(LPCWSTR title);

        LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		void DoFPS();
    };

    extern API Application& g_Application;
}