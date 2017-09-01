#pragma once

#include "cl/stdafx.h"

#include "cl/Utils/Singleton.h"
#include "cl/Utils/Time.h"

#include "Events/Events.h"

#include "cl/Memory/Vector.h"

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
		s32 UPS;
		float32 FPS, FrameTime;
	
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
		String Path;
		wchar** Args;
		s32 ArgsCount;
		bool SleepInCycle;
		void(*EntryPoint)(const String& exePath, wchar** args, s32 argsCount);
		HINSTANCE HInstance;
		HWND HWnd;
		bool WindowFocused = FALSE;
		bool Closed = FALSE;
		bool AcceptDroppedFiles;
		s32 Icon = -1;
		s32 SmallIcon = -1;
    };

    class API Application : public Singleton<Application>
	{
    private:
        ApplicationDesc m_Desc;

		Vector* m_Layers = anew Vector(sizeof(Layer*));
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

		inline const ApplicationDesc& GetDescription(void) { return m_Desc; }
		inline void SetDescription(const ApplicationDesc& settings) { m_Desc = settings; }

        void SetWindowTitle(const String& title);

        LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		void DoFPS(void);
    };

    extern API Application& g_Application;
}