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
	Texture* tex = m_texPool.Create(path);
	if (!tex)
	{
		return nullptr;
	}

	if (!tex->Load(*m_graphics, StringToWideString(path).c_str()))
	{
		return nullptr;
	}

	return tex;
}


VertexShader* ResourceManager::LoadVertexShader(const char* path, const VertexFormat& format)
{

	VertexShader* vs = m_vsPool.Create(path);
	if (!vs)
	{
		return nullptr;
	}

	if (!vs->Load(*m_graphics, StringToWideString(path).c_str(), format))
	{
		return nullptr;
	}

	return vs;
}


PixelShader* ResourceManager::LoadPixelShader(const char* path)
{
	PixelShader* ps = m_psPool.Create(path);
	if (!ps)
	{
		return nullptr;
	}

	if (!ps->Load(*m_graphics, StringToWideString(path).c_str()))
	{
		return nullptr;
	}

	return ps;
}


Model* ResourceManager::LoadModel(const char* path)
{
	Model* model = m_modelPool.Create(path);
	if (!model)
	{
		return nullptr;
	}

	if (!model->LoadFromOBJ(*m_graphics, path))
	{
		return nullptr;
	}

	return model;
}


Material* ResourceManager::CreateMaterial(const char* key, VertexShader* vs, PixelShader* ps, Texture* tex, ID3D11SamplerState* sampler, Buffer& cb)
{
	Material* mat = m_matPool.Create(key);
	if (!mat)
	{
		return nullptr;
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