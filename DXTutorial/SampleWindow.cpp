#include "SampleWindow.h"


void SampleWindow::StartUp(Graphics* graphics)
{
	// create window
	Create(L"DX Tutorial", WS_OVERLAPPEDWINDOW);

	m_swapChain = graphics->CreateSwapChain(m_hwnd);

	m_rtv = graphics->CreateRTViewFromSwapChain(m_swapChain);
}


void SampleWindow::ShutDown()
{
	if (m_rtv != NULL)
	{
		m_rtv->Release();
		m_rtv = NULL;
	}

	if (m_swapChain != NULL)
	{
		m_swapChain->Release();
		m_swapChain = NULL;
	}
}


void SampleWindow::Present()
{
	m_swapChain->Present(1, 0);
}


LRESULT SampleWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}
	return TRUE;
}