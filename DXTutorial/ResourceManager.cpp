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
	return true;
}


Texture* ResourceManager::LoadTexture(const char* path)
{
	Texture* tex = nullptr;
	StringId hash = StringHash(path);
	
	if (!m_texPool.Create(hash, tex))
	{
		return tex;
	}

	if (!tex->Load(*m_graphics, StringToWideString(path).c_str()))
	{
		return nullptr;
	}

	return tex;
}


VertexShader* ResourceManager::LoadVertexShader(const char* path, const VertexFormat& format)
{
	VertexShader* vs = nullptr;
	StringId hash = StringHash(path);

	if (!m_vsPool.Create(hash, vs))
	{
		return vs;
	}

	if (!vs->Load(*m_graphics, StringToWideString(path).c_str(), format))
	{
		return nullptr;
	}

	return vs;
}


PixelShader* ResourceManager::LoadPixelShader(const char* path)
{
	PixelShader* ps = nullptr;
	StringId hash = StringHash(path);
	
	if (!m_psPool.Create(hash, ps))
	{
		return ps;
	}

	if (!ps->Load(*m_graphics, StringToWideString(path).c_str()))
	{
		return nullptr;
	}

	return ps;
}


Model* ResourceManager::LoadModel(const char* path)
{
	Model* model = nullptr;
	StringId hash = StringHash(path);

	if (!m_modelPool.Create(hash, model))
	{
		return model;
	}

	if (!model->LoadFromOBJ(*m_graphics, path))
	{
		return nullptr;
	}

	return model;
}


Material* ResourceManager::CreateMaterial(const char* key, VertexShader* vs, PixelShader* ps, Texture* tex, ID3D11SamplerState* sampler, Buffer& cb)
{
	Material* mat = nullptr;
	StringId hash = StringHash(key);
	
	if (!m_matPool.Create(hash, mat))
	{
		return mat;
	}

	mat->SetShaders(vs, ps);
	mat->SetConstantBuffer(0, cb);

	ID3D11ShaderResourceView* srv = m_graphics->CreateShaderResource(tex->Get());
	if (srv == nullptr)
	{
		return nullptr;
	}

	mat->AddShaderInput(srv);
	mat->AddShaderSampler(sampler);

	return mat;
}