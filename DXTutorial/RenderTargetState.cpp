#include "RenderTargetState.h"


void RenderTargetState::SetRenderTarget(ID3D11RenderTargetView* rtv)
{
	m_rt = rtv;
}


void RenderTargetState::SetClearColor(bool clear, float r, float g, float b, float a)
{
	m_enableClearColor = clear;

	m_clearColor[0] = r;
	m_clearColor[1] = g;
	m_clearColor[2] = b;
	m_clearColor[3] = a;
}


void RenderTargetState::SetSize(unsigned int w, unsigned int h)
{
	m_width = w;
	m_height = h;
}


void RenderTargetState::Begin(Graphics& graphics)
{
	graphics.SetViewport(m_width, m_height);

	if (m_enableClearColor && m_rt)
	{
		graphics.ClearRenderTarget(m_rt, m_clearColor);
	}

	if (m_clearDepth || m_clearStencil)
	{
		graphics.ClearDepthStencil(m_dsv, m_enableClearDepth, m_clearDepth, m_enableClearStencil, m_clearStencil);
	}

	graphics.BindRenderTargets(&m_rt, 1, m_dsv);
}


void RenderTargetState::End(Graphics& graphics)
{
	graphics.UnbindRenderTargets();
}


void RenderTargetState::SetDepthTarget(ID3D11DepthStencilView* dsv)
{
	m_dsv = dsv;
}


void RenderTargetState::SetClearDepthStencil(bool clearDepth, float depth, bool clearStencil, unsigned char stencil)
{
	m_enableClearDepth = clearDepth;
	m_clearDepth = depth;
	m_enableClearStencil = clearStencil;
	m_clearStencil = stencil;
}