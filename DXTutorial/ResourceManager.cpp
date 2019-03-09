#include "ResourceManager.h"
#include "StringUtility.h"
#include "Texture.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Model.h"
#include "Material.h"

ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
}


bool ResourceManager::StartUp(Graphics& graphics)
{
	m_graphics = &graphics;
	m_pool.StartUp(20);
	return true;
}


bool ResourceManager::LoadTexture(const char* path, U64& handle)
{
	Texture* tex = new Texture;
	if (!tex->Load(*m_graphics, StringToWideString(path).c_str()))
	{
		return false;
	}

	const StringId hash = StringHash(path);
	handle = m_pool.Insert(hash, tex);

	return true;
}


bool ResourceManager::LoadVertexShader(const char* path, U64& handle, const VertexFormat& format)
{
	VertexShader* vs = new VertexShader;
	if (!vs->Load(*m_graphics, StringToWideString(path).c_str(), format))
	{
		return false;
	}

	const StringId hash = StringHash(path);
	handle = m_pool.Insert(hash, vs);

	return true;
}


bool ResourceManager::LoadPixelShader(const char* path, U64& handle)
{
	PixelShader* ps = new PixelShader;
	if (!ps->Load(*m_graphics, StringToWideString(path).c_str()))
	{
		return false;
	}

	const StringId hash = StringHash(path);
	handle = m_pool.Insert(hash, ps);

	return true;
}


bool ResourceManager::LoadModel(const char* path, U64& handle)
{
	Model* model = new Model;
	if (!model->LoadFromOBJ(*m_graphics, path))
	{
		return false;
	}

	const StringId hash = StringHash(path);
	handle = m_pool.Insert(hash, model);

	return true;
}


bool ResourceManager::CreateMaterial(const StringId key, U64& handle)
{
	Material* mat = new Material;
	handle = m_pool.Insert(key, mat);

	return true;
}