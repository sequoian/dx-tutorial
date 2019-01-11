#include "SampleWindow.h"


SampleWindow::SampleWindow()
{
	m_swapChain = NULL;
	m_rtv = NULL;
}


SampleWindow::~SampleWindow()
{
	ShutDown();
}


bool SampleWindow::StartUp(Graphics* graphics)
{
	// make sure startup is only called once
	assert(m_swapChain == NULL);

	// create window
	Create(L"DX Tutorial", WS_OVERLAPPEDWINDOW);

	if (!graphics->CreateSwapChain(m_hwnd))
	{
		return false;
	}

	if (!graphics->CreateRTViewFromSwapChain(m_swapChain))
	{
		return false;
	}
	
	return true;
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