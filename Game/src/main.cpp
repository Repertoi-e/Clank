#include <Clank.h>

int main()
{
	cl::g_Application.GetName() = "Clank Test";

	cl::ApplicationSettings settings;
	cl::g_Application.GetSettings(&settings);
	settings.WIDTH = 800;
	settings.HEIGHT = 600;
	settings.VSYNC = FALSE;
	settings.FULLSCREEN = FALSE;
	settings.WINDOW_STYLE = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	cl::g_Application.SetSettings(&settings);

	cl::g_Application.RegisterWindow();
	cl::g_Application.DoWindow();

	cl::g_Application.DoD3DContext();

	cl::g_Application.DoCycle();
	
	return 0;
}