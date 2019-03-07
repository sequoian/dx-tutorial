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

	bool LoadTexture(const char* path, U64& handle);
	bool LoadVertexShader(const char* path, U64& handle, const VertexFormat& format);
	bool LoadPixelShader(const char* path, U64& handle);
	bool LoadModel(const char* path, U64& handle);

	// creates an uninitialized material
	bool CreateMaterial(const char* key, U64& handle);

	// Find()

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

	// Get()

	inline Texture* GetTexture(U64 handle)
	{
		return m_texPool.Get(handle);
	}

	inline VertexShader* GetVertexShader(U64 handle)
	{
		return m_vsPool.Get(handle);
	}

	inline PixelShader* GetPixelShader(U64 handle)
	{
		return m_psPool.Get(handle);
	}

	inline Model* GetModel(U64 handle)
	{
		return m_modelPool.Get(handle);
	}

	inline Material* GetMaterial(U64 handle)
	{
		return m_matPool.Get(handle);
	}

	// Destroy()

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

	// DestroyByHandle()

	inline void DestroyTextureByHandle(U64 handle)
	{
		m_texPool.DestroyByHandle(handle);
	}

	inline void DestroyVertexShaderByHandle(U64 handle)
	{
		m_vsPool.DestroyByHandle(handle);
	}

	inline void DestroyPixelShaderByHandle(U64 handle)
	{
		m_psPool.DestroyByHandle(handle);
	}

	inline void DestroyModelByHandle(U64 handle)
	{
		m_modelPool.DestroyByHandle(handle);
	}

	inline void DestroyMaterialByHandle(U64 handle)
	{
		m_matPool.DestroyByHandle(handle);
	}

private:
	Graphics* m_graphics;

	ResourcePool<VertexShader> m_vsPool;
	ResourcePool<PixelShader> m_psPool;
	ResourcePool<Texture> m_texPool;
	ResourcePool<Model> m_modelPool;
	ResourcePool<Material> m_matPool;
};