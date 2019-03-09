#pragma once

#include "Graphics.h"
#include "Resource.h"

class VertexShader : public Resource
{
public:
	~VertexShader()
	{
		ShutDown();
	}

	virtual void ShutDown() override
	{
		if (m_shader != nullptr)
		{
			m_shader->Release();
			m_shader = nullptr;
		}

		if (m_layout != nullptr)
		{
			m_layout->Release();
			m_layout = nullptr;
		}
	}

	bool Load(Graphics& graphics, const wchar_t* fileName, const VertexFormat& format)
	{
		ID3DBlob* blob = nullptr;

		blob = graphics.CreateShaderFromFile(fileName, "vsmain", "vs_5_0");
		if (!blob)
		{
			return false;
		}

		m_shader = graphics.CreateVertexShader(blob);
		if (!m_shader)
		{
			return false;
		}

		m_layout = graphics.CreateInputLayout(blob, format);
		if (!m_layout)
		{
			return false;
		}

		blob->Release();

		return true;
	}

	ID3D11VertexShader* Get()
	{
		return m_shader;
	}

	ID3D11InputLayout* GetLayout()
	{
		return m_layout;
	}

private:
	ID3D11VertexShader* m_shader = nullptr;
	ID3D11InputLayout* m_layout = nullptr;

};