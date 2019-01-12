#include "RenderTargetState.h"


void RenderTargetState::SetRenderTarget(ID3D11RenderTargetView* rtv)
{
	m_rt = rtv;
}


void RenderTargetState::SetClearColor(bool clear, float r, float g, float b, float a)
{
	m_clear = clear;

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

	if (m_clear && m_rt)
	{
		graphics.ClearRenderTarget(m_rt, m_clearColor);
	}

	graphics.BindRenderTargets(&m_rt, 1);
}


void RenderTargetState::End(Graphics& graphics)
{
	graphics.UnbindRenderTargets();
}