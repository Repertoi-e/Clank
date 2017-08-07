#pragma once

#include "cl/stdafx.h"

#include "cl/Utils/Singleton.h"
#include "cl/Utils/Time.h"

#include "Events/Events.h"

#include <Windows.h>

namespace cl {

	class Layer;

	struct API CycleDesc
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

    struct API ApplicationDesc
    {
		String Name;
		String ClassName;
		u32 Width, Height;
        bool VSync, Fullscreen;
        u32 WindowStyle;
		CycleDesc Cycle;
    };

    class API Application : public Singleton<Application>
	{
    private:
        HINSTANCE m_hInstance;
        HWND m_hWnd;

		BOOL m_WindowFocused;
		BOOL m_Closed;

        ApplicationDesc m_Desc;

		std::vector<Layer*> m_Layers;
	public:
		Application(void);
	
		void Create(ApplicationDesc& appDesc);

		void DoWindow(void);
		void ShowWindow(void);
		void Start(void);

        void DoWindowMessages(void);
        void DoEvent(Event& event);
		void DoRender(void);
		void DoUpdate(const DeltaTime& dt);
		void DoTick(void);

		Layer* PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		inline const ApplicationDesc& GetDescription() { return m_Desc; }
		inline void SetDescription(const ApplicationDesc& settings) { m_Desc = settings; }

        void SetWindowTitle(LPCWSTR title);

        LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		void DoFPS();
    };

    extern API Application& g_Application;
}