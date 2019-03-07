#pragma once

#include "Graphics.h"

class Texture
{
public:
	~Texture()
	{
		ShutDown();
	}

	void ShutDown()
	{
		if (m_srv != nullptr)
		{
			m_srv->Release();
			m_srv = nullptr;
		}

		if (m_texture != nullptr)
		{
			m_texture->Release();
			m_texture = nullptr;
		}
	}

	bool Load(Graphics& graphics, const wchar_t* fileName)
	{
		m_texture = graphics.CreateTextureFromTGAFile(fileName);
		if (!m_texture)
		{
			return false;
		}

		m_srv = graphics.CreateShaderResource(m_texture);
		if (!m_srv)
		{
			return false;
		}

		return true;
	}

	inline ID3D11Resource* Get()
	{
		return m_texture;
	}

	inline ID3D11ShaderResourceView* GetResourceView()
	{
		return m_srv;
	}


private:
	ID3D11Resource* m_texture = nullptr;
	ID3D11ShaderResourceView* m_srv = nullptr;
};