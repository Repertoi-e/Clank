#include <Clank.h>

#include "../Resource.h"

using namespace cl;

void AppMain(const String& path, wchar** args, s32 argsCount);

// #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

s32 main()
{
	ApplicationDesc desc;
	{
		ZeroMemory(&desc, sizeof(ApplicationDesc));

		desc.Name = L"Eight Queens";
		desc.ClassName = L"Clank Application";
		desc.Width = WIDTH;
		desc.Height = HEIGHT;
		desc.WindowStyle = WS_CAPTION | WS_MINIMIZEBOX;
		desc.Icon = IDI_ICON1;
		desc.SmallIcon = IDI_ICON1_SMALL;
		desc.EntryPoint = AppMain;
		desc.LimitCycle = true;
		desc.AcceptDroppedFiles = true;

		desc.Cycle.UpdateTick = 1000.0f / 60.0f;
		desc.Cycle.Timer = anew Timer;
		desc.Cycle.UpdateTimer = 0.0f;
		desc.Cycle.UpdateDeltaTime = anew DeltaTime(0.0f);
	}
	g_Application.Create(desc);

	return EXIT_SUCCESS;
}
