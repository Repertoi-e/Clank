#include <Clank.h>

int main()
{
	cl::g_Application.GetName() = L"Clank Test Çava? Нищо, добре съм! Clank Test И ОТНОВО ПОВТАРЯМЕ Oui croissant"; 

	cl::ApplicationSettings settings;
	settings.WIDTH = 800;
	settings.HEIGHT = 600;
	settings.VSYNC = FALSE;
	settings.FULLSCREEN = FALSE;
	settings.WINDOW_STYLE = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	cl::g_Application.SetSettings(&settings);

	cl::g_Application.DoWindow();

	cl::CycleInfo info;
	ZeroMemory(&info, sizeof(info));
	info.UpdateTick = 1000.0f / 60.0f;
	cl::g_Application.SetCycleInfo(&info);

	cl::g_Application.DoCycle();
	
	return 0;
}