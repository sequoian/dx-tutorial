#include "SampleApplication.h"

class ClearSample : public SampleApplication
{
public:
	virtual void Render() override
	{
		float clearColor[4] = { 0, 1, 0.75, 1 };
		m_graphics.ClearRenderTarget(m_window.GetRenderTargetView(), clearColor);
	}
};


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	ClearSample sample;
	if (!sample.StartUp())
	{
		MessageBox(NULL, L"Failed to initialize the application", L"ERROR", MB_OK);
		return 1;
	}
	sample.Run();
	sample.ShutDown();

	return 0;
}
