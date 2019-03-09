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


bool ResourceManager::LoadTexture(const char* path, U64& handle, const StringId key)
{
	Texture* tex = new Texture;
	if (!tex->Load(*m_graphics, StringToWideString(path).c_str()))
	{
		return false;
	}

	handle = m_pool.Insert(key, tex);

	return true;
}


bool ResourceManager::LoadVertexShader(const char* path, U64& handle, const VertexFormat& format, const StringId key)
{
	VertexShader* vs = new VertexShader;
	if (!vs->Load(*m_graphics, StringToWideString(path).c_str(), format))
	{
		return false;
	}

	handle = m_pool.Insert(key, vs);

	return true;
}


bool ResourceManager::LoadPixelShader(const char* path, U64& handle, const StringId key)
{
	PixelShader* ps = new PixelShader;
	if (!ps->Load(*m_graphics, StringToWideString(path).c_str()))
	{
		return false;
	}

	handle = m_pool.Insert(key, ps);

	return true;
}


bool ResourceManager::LoadModel(const char* path, U64& handle, const StringId key)
{
	Model* model = new Model;
	if (!model->LoadFromOBJ(*m_graphics, path))
	{
		return false;
	}

	handle = m_pool.Insert(key, model);

	return true;
}


bool ResourceManager::CreateMaterial(U64& handle, const StringId key)
{
	Material* mat = new Material;
	handle = m_pool.Insert(key, mat);

	return true;
}