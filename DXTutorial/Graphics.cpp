#include "Graphics.h"


Graphics::Graphics()
{
	m_device = NULL;
	m_context = NULL;
}


Graphics::~Graphics()
{
	ShutDown();
}


bool Graphics::StartUp()
{
	// make sure startup is only called once
	assert(m_device == NULL);

	ID3D11Device* device = NULL;
	ID3D11DeviceContext* context = NULL;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	HRESULT hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&device,
		NULL,
		&context);
	if (hr != S_OK)
	{
		// failed to create the D3D device (usually means no hardware support)
		WriteLog("Failed to create the D3D device\n");
		return false;
	}
	else
	{
		// device creation succeeded
		m_device = device;
		m_context = context;
	}

	return true;
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
		WriteLog("Failed to get DXGI device\n");
		return NULL;
	}

	// Get the adapter interface from the DXGI device (AKA the video card interface)
	IDXGIAdapter* adapter = NULL;
	hr = dxgidevice->GetAdapter(&adapter);
	if (hr != S_OK)
	{
		WriteLog("Failed to get DXGI interface\n");
		return NULL;
	}

	// Get the DXGI factory from the adapter (we need this to create a swap chain)
	IDXGIFactory* factory = NULL;
	hr = adapter->GetParent(IID_PPV_ARGS(&factory));
	if (hr != S_OK)
	{
		WriteLog("Failed to get DXGI factory\n");
		return NULL;
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
		WriteLog("Failed to create the swap chain\n");
		return NULL;
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
		WriteLog("Failed to get back buffer from swap chain\n");
		return NULL;
	}

	// Create a RenderTargetView (RTV) from the buffer resource.
	// We need this to do any rendering to the back buffer
	ID3D11RenderTargetView* rtv = NULL;
	hr = m_device->CreateRenderTargetView(buffer, NULL, &rtv);
	if (hr != S_OK)
	{
		WriteLog("Failed to create RTV\n");
		return NULL;
	}

	// We're done with the buffer now
	buffer->Release();

	return rtv;
}


void Graphics::ClearRenderTarget(ID3D11RenderTargetView* rtv, float rgba[4])
{
	m_context->ClearRenderTargetView(rtv, rgba);
}


ID3DBlob* Graphics::CreateShaderFromFile(const wchar_t* fileName, const char* entryPoint, const char* target)
{
	ID3DBlob* codeBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		fileName,
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint,
		target,
		D3DCOMPILE_OPTIMIZATION_LEVEL3,
		0,
		&codeBlob,
		&errorBlob
	);

	if (hr != S_OK)
	{
		// handle error
		if (errorBlob != nullptr)
		{
			WriteLog((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		else
		{
			WriteLog("Failed to create shader from file\n");
		}

		return nullptr;
	}

	return codeBlob;
}


ID3D11VertexShader* Graphics::CreateVertexShader(ID3DBlob* vsCode)
{
	ID3D11VertexShader* vertexShader = nullptr;

	HRESULT hr = m_device->CreateVertexShader(
		vsCode->GetBufferPointer(),
		vsCode->GetBufferSize(),
		NULL, &vertexShader
	);

	if (hr != S_OK)
	{
		WriteLog("Failed to create vertex shader\n");
		return nullptr;
	}

	return vertexShader;
}


ID3D11PixelShader* Graphics::CreatePixelShader(ID3DBlob* psCode)
{
	ID3D11PixelShader* pixelShader = nullptr;

	HRESULT hr = m_device->CreatePixelShader(
		psCode->GetBufferPointer(), 
		psCode->GetBufferSize(), 
		NULL, &pixelShader
	);

	if (hr != S_OK)
	{
		WriteLog("Failed to create pixel shader\n");
		return nullptr;
	}

	return pixelShader;
}

ID3D11InputLayout* Graphics::CreateInputLayout(ID3DBlob* vsCode, const VertexFormat& vbFmt)
{
	ID3D11InputLayout* layout = nullptr;

	HRESULT hr = m_device->CreateInputLayout(
		vbFmt.Inputs,
		vbFmt.NumInputs,
		vsCode->GetBufferPointer(),
		vsCode->GetBufferSize(),
		&layout
	);

	if (hr != S_OK)
	{
		WriteLog("Failed to create input layout\n");
		return nullptr;
	}

	return layout;
}


ID3D11Buffer* Graphics::CreateVertexBuffer(unsigned int numVerts, unsigned int stride,
	bool dynamic, bool gpuwrite, const void* data)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = numVerts * stride;
	desc.StructureByteStride = stride;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (dynamic)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (gpuwrite)
	{
		desc.Usage = D3D11_USAGE_DEFAULT;
	}
	else
	{
		desc.Usage = D3D11_USAGE_IMMUTABLE;
	}

	D3D11_SUBRESOURCE_DATA initdata = {};
	initdata.pSysMem = data;
	initdata.SysMemPitch = numVerts * stride;

	ID3D11Buffer* vb = nullptr;
	HRESULT hr = m_device->CreateBuffer(
		&desc,
		data == nullptr ? nullptr : &initdata,
		&vb
	);

	if (hr != S_OK)
	{
		WriteLog("Failed to create vertex buffer\n");
		return nullptr;
	}
		
	return vb;
}

void Graphics::SetViewport(unsigned int width, unsigned int height)
{
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	m_context->RSSetViewports(1, &viewport);
}


void Graphics::BindRenderTargets(ID3D11RenderTargetView** rts, unsigned int numRTs)
{
	m_context->OMSetRenderTargets(numRTs, rts, NULL);

}


void Graphics::UnbindRenderTargets()
{
	m_context->OMSetRenderTargets(0, NULL, NULL);
}


void Graphics::SetVertexBuffer(ID3D11Buffer* vb, unsigned int slot, unsigned int stride, unsigned int offset)
{
	m_context->IASetVertexBuffers(slot, 1, &vb, &stride, &offset);
}


void Graphics::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	m_context->IASetPrimitiveTopology(topology);
}


void Graphics::SetInputLayout(ID3D11InputLayout* layout)
{
	m_context->IASetInputLayout(layout);
}


void Graphics::SetVertexShader(ID3D11VertexShader* vs)
{
	m_context->VSSetShader(vs, NULL, 0);
}


void Graphics::SetPixelShader(ID3D11PixelShader* ps)
{
	m_context->PSSetShader(ps, NULL, 0);
}


void Graphics::Draw(unsigned int numVertices, unsigned int startVertex)
{
	m_context->Draw(numVertices, startVertex);
}