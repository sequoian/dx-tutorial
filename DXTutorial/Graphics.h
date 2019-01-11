#pragma once

#include <d3d11.h>
#include <cassert>
#include "WriteLog.h"

class Graphics
{
private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

public:
	Graphics();
	~Graphics();
	bool StartUp();
	void ShutDown();
	IDXGISwapChain* CreateSwapChain(HWND hwnd);
	ID3D11RenderTargetView* CreateRTViewFromSwapChain(IDXGISwapChain* swapChain);
	void ClearRenderTarget(ID3D11RenderTargetView* rtv, float rgba[4]);
};