#include "SampleApplication.h"


class ClearSample : public SampleApplication
{
public:
	void Render()
	{
		float clearColor[4] = { 0, 1, 0.75, 1 };
		m_graphics.ClearRenderTarget(m_window.GetRenderTargetView(), clearColor);
	}
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	ClearSample sample;
	sample.StartUp();
	sample.Run();
	sample.ShutDown();
}
