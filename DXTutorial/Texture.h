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
		if (m_texture != nullptr)
		{
			m_texture->Release();
		}
	}

	bool Load(Graphics& graphics, const wchar_t* fileName)
	{
		m_texture = graphics.CreateTextureFromTGAFile(fileName);
		if (!m_texture)
		{
			return false;
		}

		return true;
	}

	ID3D11Resource* Get()
	{
		return m_texture;
	}


private:
	ID3D11Resource* m_texture = nullptr;
};