#pragma once

#include "Graphics.h"
#include "ResourcePool.h"
#include "StringId.h"

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

	inline Texture* FindTexture(const StringId key)
	{
		return m_texPool.Find(key);
	}

	inline VertexShader* FindVertexShader(const StringId key)
	{
		return m_vsPool.Find(key);
	}

	inline PixelShader* FindPixelShader(const StringId key)
	{
		return m_psPool.Find(key);
	}

	inline Model* FindModel(const StringId key)
	{
		return m_modelPool.Find(key);
	}

	inline Material* FindMaterial(const StringId key)
	{
		return m_matPool.Find(key);
	}

	inline void DestroyTexture(const StringId key)
	{
		m_texPool.Destroy(key);
	}

	inline void DestroyVertexShader(const StringId key)
	{
		m_vsPool.Destroy(key);
	}

	inline void DestroyPixelShader(const StringId key)
	{
		m_psPool.Destroy(key);
	}

	inline void DestroyModel(const StringId key)
	{
		m_modelPool.Destroy(key);
	}

	inline void DestroyMaterial(const StringId key)
	{
		m_matPool.Destroy(key);
	}

private:
	Graphics* m_graphics;

	ResourcePool<VertexShader> m_vsPool;
	ResourcePool<PixelShader> m_psPool;
	ResourcePool<Texture> m_texPool;
	ResourcePool<Model> m_modelPool;
	ResourcePool<Material> m_matPool;
};