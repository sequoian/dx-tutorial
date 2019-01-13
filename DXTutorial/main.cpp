#include "SampleApplication.h"
#include "RenderTargetState.h"
#include "Material.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Model.h"
#include "VertexFormat.h"

#include <DirectXMath.h>
using namespace DirectX;

struct ModelConstants
{
	XMMATRIX m_world;
	XMMATRIX m_viewproj;
};

class ModelSample : public SampleApplication
{
public:
	virtual ~ModelSample()
	{
		if (m_sampler != nullptr)
			m_sampler->Release();
		if (m_srv != nullptr)
			m_srv->Release();
		if (m_tex != nullptr)
			m_tex->Release();
	}

	virtual bool StartUp() override
	{
		if (!SampleApplication::StartUp())
			return false;

		if (!m_cb.CreateConstantBuffer(m_graphics, sizeof(ModelConstants), true,
			nullptr))
			return false;

		if (!m_material.Load(m_graphics, L"Shaders/tutorial5.hlsl",
			VertPosNormUVColor::GetVertexFormat()))
			return false;

		if (!m_model.LoadFromOBJ(m_graphics, "Assets/monkey.obj"))
			return false;

		m_tex = m_graphics.CreateTextureFromTGAFile(L"Assets/stone.tga");
		if (m_tex == nullptr)
			return false;

		m_srv = m_graphics.CreateShaderResource(m_tex);
		if (m_srv == nullptr)
			return false;

		D3D11_SAMPLER_DESC sampler;
		sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler.AddressU =
			sampler.AddressV =
			sampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler.MinLOD = -FLT_MAX;
		sampler.MaxLOD = FLT_MAX;
		sampler.MipLODBias = 0.0f;
		sampler.MaxAnisotropy = 1;
		sampler.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampler.BorderColor[0] =
			sampler.BorderColor[1] =
			sampler.BorderColor[2] =
			sampler.BorderColor[3] = 1.0f;

		m_sampler = m_graphics.CreateSampler(sampler);
		if (m_sampler == nullptr)
			return false;

		m_material.SetConstantBuffer(0, m_cb);
		m_material.AddShaderInput(m_srv);
		m_material.AddShaderSampler(m_sampler);

		m_rtState.SetRenderTarget(m_window.GetRenderTarget());
		m_rtState.SetClearColor(true, 0.0f, 0.0f, 0.0f, 1.0f);
		m_rtState.SetSize(m_window.GetScreenWidth(), m_window.GetScreenHeight());

		return true;
	}

	virtual void Update() override
	{
		m_time += 1.0f / 60.0f;
	}

	virtual void Render() override
	{
		// hard-coded camera position and target
		XMVECTOR eyepos = XMVectorSet(0.0f, 2.0f, -5.0f, 1.0f);
		XMVECTOR target = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		// generate view and projection matrices
		XMMATRIX view = XMMatrixLookAtLH(eyepos, target, XMVectorSet(0.0f, 1.0f, 0.0f,
			0.0f));
		XMMATRIX proj = XMMatrixPerspectiveFovLH(45.0f, m_window.GetScreenWidth() /
			float(m_window.GetScreenHeight()), 0.01f, 1000.0f);

		// update our constants with data for this frame
		ModelConstants consts;
		consts.m_world = XMMatrixRotationRollPitchYaw(0.0f, m_time * 0.5f, 0.0f);
		consts.m_viewproj = XMMatrixMultiply(view, proj);
		m_cb.MapAndSet(m_graphics, consts);

		m_rtState.Begin(m_graphics);
		m_model.Select(m_graphics);
		m_material.Select(m_graphics);
		m_model.Draw(m_graphics);
		m_rtState.End(m_graphics);
	}

private:
	RenderTargetState m_rtState;
	Material m_material;
	Model m_model;
	Buffer m_cb;
	ID3D11Resource* m_tex = nullptr;
	ID3D11ShaderResourceView* m_srv = nullptr;
	ID3D11SamplerState* m_sampler = nullptr;
	float m_time = 0.0f;
};




int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	ModelSample sample;
	if (!sample.StartUp())
	{
		MessageBox(NULL, L"Failed to initialize the application", L"ERROR", MB_OK);
		return 1;
	}
	sample.Run();
	sample.ShutDown();

	return 0;
}
