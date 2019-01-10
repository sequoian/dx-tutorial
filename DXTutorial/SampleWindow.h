#pragma once

#include <windows.h>
#include "BaseWindow.h"
#include "Graphics.h"

class SampleWindow : public BaseWindow<SampleWindow>
{
private:
	IDXGISwapChain* m_swapChain;
	ID3D11RenderTargetView* m_rtv;

public:
	SampleWindow();
	~SampleWindow();
	PCWSTR  ClassName() const { return L"Sample Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void StartUp(Graphics* graphics);
	void ShutDown();
	void Present();
	ID3D11RenderTargetView* GetRenderTargetView() const { return m_rtv; }
};