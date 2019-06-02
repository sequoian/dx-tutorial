#pragma once

#include "Graphics.h"

class RenderTargetState
{
public:
	void SetRenderTarget(ID3D11RenderTargetView* rtv);
	void SetDepthTarget(ID3D11DepthStencilView* dsv);
	void SetClearColor(bool clear, float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f);
	void SetSize(unsigned int w, unsigned int h);
	void SetClearDepthStencil(bool clearDepth, float depth = 1.0f, bool clearStencil = false, unsigned char stencil = 0);


	void Begin(Graphics& graphics);
	void End(Graphics& graphics);

private:
	ID3D11RenderTargetView * m_rt = nullptr;
	ID3D11DepthStencilView* m_dsv = nullptr;
	float m_clearColor[4] = {};
	float m_clearDepth = 1.0f;
	unsigned int m_width = 0;
	unsigned int m_height = 0;
	bool m_enableClearColor = true;
	bool m_enableClearDepth = false;
	bool m_enableClearStencil = false;
	unsigned char m_clearStencil = 0;

};
