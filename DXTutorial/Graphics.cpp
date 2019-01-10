#include "Graphics.h"


Graphics::Graphics()
{
}


Graphics::~Graphics()
{
	ShutDown();
}


void Graphics::StartUp()
{
	static bool called;
	if (called)
	{
		assert(0);
		return;
	}
	called = true;

	ID3D11Device* device = NULL;
	ID3D11DeviceContext* context = NULL;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&device,
		NULL,
		&context);
	if (hr != S_OK)
	{
		// failed to create the D3D device (usually means no hardware support)
		MessageBox(NULL, L"Failed to create the D3D device", NULL, MB_OK);
	}
	else
	{
		// device creation succeeded
		m_device = device;
		m_context = context;
	}
}


void Graphics::ShutDown()
{
	if (m_device != NULL)
	{
		m_device->Release();
		m_device = NULL;
	}

	if (m_context != NULL)
	{
		m_context->Release();
		m_context = NULL;
	}
}


IDXGISwapChain* Graphics::CreateSwapChain(HWND hwnd)
{
	// Get the DXGI device from the D3D device
	IDXGIDevice* dxgidevice = NULL;
	HRESULT hr = m_device->QueryInterface(IID_PPV_ARGS(&dxgidevice));
	if (hr != S_OK)
	{
		MessageBox(NULL, L"Failed to get DXGI device", NULL, MB_OK);
	}

	// Get the adapter interface from the DXGI device (AKA the video card interface)
	IDXGIAdapter* adapter = NULL;
	hr = dxgidevice->GetAdapter(&adapter);
	if (hr != S_OK)
	{
		MessageBox(NULL, L"Failed to get DXGI interface", NULL, MB_OK);
	}

	// Get the DXGI factory from the adapter (we need this to create a swap chain)
	IDXGIFactory* factory = NULL;
	hr = adapter->GetParent(IID_PPV_ARGS(&factory));
	if (hr != S_OK)
	{
		MessageBox(NULL, L"Failed to get DXGI factory", NULL, MB_OK);
	}

	// Use this function to get the current dimensions of the window
	RECT wndRect;
	GetClientRect(hwnd, &wndRect);

	// To create a swap chain, we need to provide DX information about how to render
	DXGI_SWAP_CHAIN_DESC scdesc = {};
	// The window the swap chain attaches to
	scdesc.OutputWindow = hwnd;
	// The number of "back buffers" in the swap chain (for windowed mode, only need 1)
	scdesc.BufferCount = 1;
	// The bit-depth and encoding of the back buffers (8-bits per channel, RGBA)
	scdesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// width of the back buffer
	scdesc.BufferDesc.Width = wndRect.right;
	// height of the back buffer
	scdesc.BufferDesc.Height = wndRect.bottom;
	// these are for the refresh rate, with windowed, we set it to 0, 1
	scdesc.BufferDesc.RefreshRate.Numerator = 0;
	scdesc.BufferDesc.RefreshRate.Denominator = 1;
	// if the window is larger than the back buffer, stretch out
	scdesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	// progressive scan mode
	scdesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	// tell DX we want to use it as a back buffer AND a render target
	scdesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// We don't want multisampling on this guy
	scdesc.SampleDesc.Count = 1;
	scdesc.SampleDesc.Quality = 0;
	// Use the basic swap mode
	scdesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	// We only want to run in windowed mode for now (not full-screen)
	scdesc.Windowed = TRUE;

	// Create the swap chain
	IDXGISwapChain* swapChain = NULL;
	hr = factory->CreateSwapChain(m_device, &scdesc, &swapChain);

	// Release these interfaces since we're done with them
	dxgidevice->Release();
	adapter->Release();
	factory->Release();

	if (hr != S_OK)
	{
		// Handle error. failed to create swap chain
		MessageBox(NULL, L"Failed to create the swap chain", NULL, MB_OK);
	}

	return swapChain;
}


ID3D11RenderTargetView* Graphics::CreateRTViewFromSwapChain(IDXGISwapChain* swapChain)
{
	// Get the back buffer resource from the swap chain
	ID3D11Resource* buffer = NULL;
	HRESULT hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&buffer));
	if (hr != S_OK)
	{
		MessageBox(NULL, L"Failed to get back buffer from swap chain", NULL, MB_OK);
	}

	// Create a RenderTargetView (RTV) from the buffer resource.
	// We need this to do any rendering to the back buffer
	ID3D11RenderTargetView* rtv = NULL;
	hr = m_device->CreateRenderTargetView(buffer, NULL, &rtv);
	if (hr != S_OK)
	{
		MessageBox(NULL, L"Failed to create RTV", NULL, MB_OK);
	}

	// We're done with the buffer now
	buffer->Release();

	return rtv;
}


void Graphics::ClearRenderTarget(ID3D11RenderTargetView* rtv, float rgba[4])
{
	m_context->ClearRenderTargetView(rtv, rgba);
}