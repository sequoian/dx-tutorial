#pragma once

#include "Graphics.h"
#include "ResourcePool.h"

#include "VertexShader.h"
#include "VertexFormat.h"
#include "PixelShader.h"
#include "Texture.h"
#include "Model.h"
#include "Material.h"


class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	bool StartUp(Graphics& graphics);

	Texture* LoadTexture(const char* path);
	VertexShader* LoadVertexShader(const char* path, const VertexFormat& format);
	PixelShader* LoadPixelShader(const char* path);
	Model* LoadModel(const char* path);
	Material* CreateMaterial(const char* key, VertexShader* vs, PixelShader* ps, Texture* tex, ID3D11SamplerState* sampler, Buffer& cb);

	Texture* FindTexture(const char* key)
	{
		return m_texPool.Find(key);
	}

	VertexShader* FindVertexShader(const char* key)
	{
		return m_vsPool.Find(key);
	}

	PixelShader* FindPixelShader(const char* key)
	{
		return m_psPool.Find(key);
	}

	Model* FindModel(const char* key)
	{
		m_modelPool.Find(key);
	}

	Material* FindMaterial(const char* key)
	{
		m_matPool.Find(key);
	}

private:
	Graphics* m_graphics;

	ResourcePool<VertexShader> m_vsPool;
	ResourcePool<PixelShader> m_psPool;
	ResourcePool<Texture> m_texPool;
	ResourcePool<Model> m_modelPool;
	ResourcePool<Material> m_matPool;
};