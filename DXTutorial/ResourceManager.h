#pragma once

#include "Graphics.h"
#include "ResourcePool.h"
#include "StringId.h"
#include "Resource.h"


class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	bool StartUp(Graphics& graphics);

	bool LoadTexture(const char* path, U64& handle, const StringId key);
	bool LoadVertexShader(const char* path, U64& handle, const VertexFormat& format, const StringId key);
	bool LoadPixelShader(const char* path, U64& handle, const StringId key);
	bool LoadModel(const char* path, U64& handle, const StringId key);

	// creates an uninitialized material
	bool CreateMaterial(U64& handle, const StringId key);


	inline Resource* FindResourceByStringId(const StringId key)
	{
		return m_pool.Find(key);
	}


	inline Resource* GetResourceByHandle(U64 handle)
	{
		return m_pool.Get(handle);
	}


	inline void DestroyResource(const StringId key)
	{
		m_pool.DestroyByStringId(key);
	}


private:
	Graphics* m_graphics;
	ResourcePool m_pool;
};