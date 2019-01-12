#pragma once

#include <d3d11.h>
#include <cassert>
#include <d3dcompiler.h>
#include "WriteLog.h"
#include "VertexFormat.h"

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
	ID3DBlob* CreateShaderFromFile(const wchar_t* fileName, const char* entryPoint, const char* target);
	ID3D11VertexShader* CreateVertexShader(ID3DBlob* vsCode);
	ID3D11PixelShader* CreatePixelShader(ID3DBlob* psCode);
	ID3D11InputLayout* CreateInputLayout(ID3DBlob* vsCode, const VertexFormat& vbFmt);
	ID3D11Buffer* CreateVertexBuffer(unsigned int numVerts, unsigned int stride,
		bool dynamic, bool gpuwrite, const void* data);
	void SetViewport(unsigned int width, unsigned int height);
	void BindRenderTargets(ID3D11RenderTargetView** rts, unsigned int numRTs);
	void UnbindRenderTargets();
	void SetVertexBuffer(ID3D11Buffer* vb, unsigned int slot, unsigned int stride,
		unsigned int offset = 0);
	void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
	void SetInputLayout(ID3D11InputLayout* layout);
	void SetVertexShader(ID3D11VertexShader* vs);
	void SetPixelShader(ID3D11PixelShader* ps);
	void Draw(unsigned int numVertices, unsigned int startVertex = 0);

};