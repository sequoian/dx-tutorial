#pragma once

#include "Graphics.h"

class PixelShader
{
public:
	~PixelShader()
	{
		ShutDown();
	}

	void ShutDown()
	{
		if (m_shader != nullptr)
		{
			m_shader->Release();
			m_shader = nullptr;
		}
	}

	bool Load(Graphics& graphics, const wchar_t* fileName)
	{
		ID3DBlob* blob = nullptr;

		blob = graphics.CreateShaderFromFile(fileName, "psmain", "ps_5_0");
		if (!blob)
		{
			return false;
		}

		m_shader = graphics.CreatePixelShader(blob);
		if (!m_shader)
		{
			return false;
		}

		blob->Release();

		return true;
	}

	ID3D11PixelShader* Get()
	{
		return m_shader;
	}

private:
	ID3D11PixelShader * m_shader = nullptr;

};