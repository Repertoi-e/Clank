#include <Clank.h>

int main()
{
	cl::Application* app = new cl::Application("Clank", { 800, 600, TRUE, FALSE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX });
	app->Start();

	return 0;
}