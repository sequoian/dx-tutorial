#include "SampleApplication.h"
#include "RenderTargetState.h"
#include "Material.h"
#include "VertexBuffer.h"

class TriSample : public SampleApplication
{
public:
	bool StartUp()
	{
		if (!SampleApplication::StartUp())
		{
			return false;
		}

		if (!m_material.Load(m_graphics, L"Shaders/tutorial3.hlsl", VertPosColor::GetVertexFormat()))
		{
			return false;
		}

		VertPosColor vbData[3] = {};
		vbData[0].x = -0.25f;
		vbData[0].y = -0.25f;
		vbData[0].z = 0.0f;
		vbData[0].color = MakeColorUInt(255, 0, 0, 255);

		vbData[1].x = 0.0f;
		vbData[1].y = 0.25f;
		vbData[1].z = 0.0f;
		vbData[1].color = MakeColorUInt(0, 255, 0, 255);

		vbData[2].x = 0.25f;
		vbData[2].y = -0.25f;
		vbData[2].z = 0.0f;
		vbData[2].color = MakeColorUInt(0, 0, 255, 255);

		if (!m_vb.StartUp(m_graphics, VertPosColor::GetVertexFormat(), 3, false, false, &vbData))
		{
			return false;
		}
			

		m_rtState.SetRenderTarget(m_window.GetRenderTargetView());
		m_rtState.SetClearColor(true, 1.0f, 0.0f, 0.0f, 1.0f);
		m_rtState.SetSize(m_window.GetScreenWidth(), m_window.GetScreenHeight());

		return true;
	}

	virtual void Render() override
	{
		m_rtState.Begin(m_graphics);
		m_vb.Select(m_graphics, 0);
		m_material.Select(m_graphics);
		m_graphics.Draw(3);
		m_rtState.End(m_graphics);

	}

private:
	Material m_material;
	VertexBuffer m_vb;
	RenderTargetState m_rtState;
};


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	TriSample sample;
	if (!sample.StartUp())
	{
		MessageBox(NULL, L"Failed to initialize the application", L"ERROR", MB_OK);
		return 1;
	}
	sample.Run();
	sample.ShutDown();

	return 0;
}
