#pragma once

#include "Graphics.h"

class RenderTargetState
{
public:
	void SetRenderTarget(ID3D11RenderTargetView* rtv);
	void SetClearColor(bool clear, float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f);
	void SetSize(unsigned int w, unsigned int h);

	void Begin(Graphics& graphics);
	void End(Graphics& graphics);

private:
	ID3D11RenderTargetView * m_rt = nullptr;
	float m_clearColor[4] = {};
	unsigned int m_width = 0;
	unsigned int m_height = 0;
	bool m_clear = true;
};
