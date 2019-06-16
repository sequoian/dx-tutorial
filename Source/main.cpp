#include "ThirdPersonApp.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	ThirdPersonApp app;
	if (!app.StartUp())
	{
		MessageBox(NULL, L"Failed to initialize the application", L"ERROR", MB_OK);
		return 1;
	}
	app.Run();
	app.ShutDown();
	return 0;
}
