#pragma once

#include "cl/stdafx.h"

#include "cl/Utils/Singleton.h"
#include "cl/Utils/Time.h"

#include "Events/Events.h"

#include "cl/Memory/Vector.h"

#include <Windows.h>

namespace cl {

	class Scene;

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
		HINSTANCE HInstance;
		HWND HWnd;
		String Name;
		String ClassName;
		u32 Width, Height;
		u32 WindowStyle;
		CycleDesc Cycle;
		bool LimitCycle;
		bool VSync;
		bool Fullscreen;
		bool WindowFocused = false;
		bool Closed = false;
		bool AcceptDroppedFiles;
		String Path;
		wchar** Args;
		s32 ArgsCount; 
		void(*EntryPoint)(const String& path, wchar** args, s32 argsCount);
		s32 Icon = -1;
		s32 SmallIcon = -1;
    };

    class API Application : public Singleton<Application>
	{
    private:
        ApplicationDesc m_Desc;

		Vector<Scene*> m_Scenes;
	public:
		Application(void);

		void Create(const ApplicationDesc& appDesc);

		void DoWindow(void);
		void ShowWindow(void);
		void Start(void);

        void DoWindowMessages(void);
        void DoEvent(Event& event);
		void DoRender(void);
		void DoUpdate(const DeltaTime& dt);
		void DoTick(void);

		Scene* PushScene(Scene* scene);
		void PopScene(Scene* scene);
		
		void SetWindowTitle(const String& title);

		inline ApplicationDesc& GetDesc(void) { return m_Desc; }
		inline const ApplicationDesc& GetDesc(void) const { return m_Desc; }

        LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		void DoFPS(void);
    };

    extern API Application& g_Application;
	extern API ApplicationDesc& g_ApplicationDesc;
}