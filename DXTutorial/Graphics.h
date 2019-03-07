#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include "WriteLog.h"
#include "VertexFormat.h"

class Graphics
{
private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	// samplers
	ID3D11SamplerState* m_linearWrapSampler;

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
	void BindRenderTargets(ID3D11RenderTargetView** rts, unsigned int numRTs, ID3D11DepthStencilView* dsv = nullptr);
	void UnbindRenderTargets();
	void SetVertexBuffer(ID3D11Buffer* vb, unsigned int slot, unsigned int stride,
		unsigned int offset = 0);
	void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
	void SetInputLayout(ID3D11InputLayout* layout);
	void SetVertexShader(ID3D11VertexShader* vs);
	void SetPixelShader(ID3D11PixelShader* ps);
	void Draw(unsigned int numVertices, unsigned int startVertex = 0);
	ID3D11Buffer* CreateIndexBuffer(unsigned int numIndices, bool dynamic, bool gpuwrite, const unsigned int* data);
	ID3D11Buffer* CreateConstantBuffer(unsigned int size, bool dynamic, const void* data);
	ID3D11Resource* CreateTextureFromTGAFile(const wchar_t* fileName);
	ID3D11ShaderResourceView* CreateShaderResource(ID3D11Resource* res);
	ID3D11SamplerState* CreateSampler(const D3D11_SAMPLER_DESC& samplerInfo);
	void SetIndexBuffer(ID3D11Buffer* ib, unsigned int offset = 0);
	void* MapBuffer(ID3D11Buffer* buffer);
	void UnmapBuffer(ID3D11Buffer* buffer);
	void SetVSShaderInputs(unsigned int slot, unsigned int numSlots, ID3D11ShaderResourceView* srvs[]);
	void SetPSShaderInputs(unsigned int slot, unsigned int numSlots, ID3D11ShaderResourceView* srvs[]);
	void SetVSConstantBuffers(unsigned int slot, unsigned int numSlots, ID3D11Buffer* buffers[]);
	void SetPSConstantBuffers(unsigned int slot, unsigned int numSlots, ID3D11Buffer* buffers[]);
	void SetVSSamplers(unsigned int slot, unsigned int numSlots, ID3D11SamplerState* samplers[]);
	void SetPSSamplers(unsigned int slot, unsigned int numSlots, ID3D11SamplerState* samplers[]);
	void DrawIndexed(unsigned int numIndices, unsigned int startIndex = 0, unsigned int startVertex = 0);
	ID3D11Texture2D* CreateDepthBuffer(unsigned int width, unsigned int height, DXGI_FORMAT fmt);
	ID3D11DepthStencilView* CreateDepthStencilView(ID3D11Texture2D* tex);
	ID3D11DepthStencilState* CreateDepthStencilState( const D3D11_DEPTH_STENCIL_DESC& desc);
	void ClearDepthStencil(ID3D11DepthStencilView* dsv, bool clearDepth, float depth, bool clearStencil, unsigned char stencil);
	void SetDepthStencilState(ID3D11DepthStencilState* dss, unsigned int stencilRef = 0);
	ID3D11SamplerState* GetLinearWrapSampler();
};