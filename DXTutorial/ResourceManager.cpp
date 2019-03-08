#include "ResourceManager.h"
#include "StringUtility.h"

ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
}


bool ResourceManager::StartUp(Graphics& graphics)
{
	m_graphics = &graphics;
	m_texPool.StartUp(10);
	m_vsPool.StartUp(10);
	m_psPool.StartUp(10);
	m_modelPool.StartUp(10);
	m_matPool.StartUp(10);
	return true;
}


bool ResourceManager::LoadTexture(const char* path, U64& handle)
{
	const StringId hash = StringHash(path);
	if (!m_texPool.Create(hash, handle))
	{
		return true;
	}
	
	Texture* tex = m_texPool.Get(handle);
	if (!tex->Load(*m_graphics, StringToWideString(path).c_str()))
	{
		return false;
	}

	return true;
}


bool ResourceManager::LoadVertexShader(const char* path, U64& handle, const VertexFormat& format)
{
	const StringId hash = StringHash(path);
	if (!m_vsPool.Create(hash, handle))
	{
		return true;
	}

	VertexShader* vs = m_vsPool.Get(handle);
	if (!vs->Load(*m_graphics, StringToWideString(path).c_str(), format))
	{
		return false;
	}

	return true;
}


bool ResourceManager::LoadPixelShader(const char* path, U64& handle)
{
	const StringId hash = StringHash(path);
	if (!m_psPool.Create(hash, handle))
	{
		return true;
	}

	PixelShader* ps = m_psPool.Get(handle);
	if (!ps->Load(*m_graphics, StringToWideString(path).c_str()))
	{
		return false;
	}

	return true;
}


bool ResourceManager::LoadModel(const char* path, U64& handle)
{
	const StringId hash = StringHash(path);
	if (!m_modelPool.Create(hash, handle))
	{
		return true;
	}

	Model* model = m_modelPool.Get(handle);
	if (!model->LoadFromOBJ(*m_graphics, path))
	{
		return false;
	}

	return true;
}


bool ResourceManager::CreateMaterial(const StringId key, U64& handle)
{
	m_matPool.Create(key, handle);
	return true;
}