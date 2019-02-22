#include "SampleApplication.h"
#include "RenderTargetState.h"
#include "Material.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Model.h"
#include "VertexFormat.h"
#include "Timer.h"
#include "Gamepad.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "EntityManager.h"
#include "TransformSystem.h"
#include "RotatorSystem.h"
#include "CameraSystem.h"

#include <DirectXMath.h>
using namespace DirectX;

struct ModelConstants
{
	XMMATRIX m_world;
	XMMATRIX m_viewproj;
	XMVECTOR m_cameraPos;
	XMVECTOR m_lightDirection;
	XMVECTOR m_lightColor;
	XMVECTOR m_ambientColor;
	XMVECTOR m_specularColor;
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
		if (m_dss != nullptr)
			m_dss->Release();
		if (m_dsv != nullptr)
			m_dsv->Release();
		if (m_depth != nullptr)
			m_depth->Release();
	}

	virtual bool StartUp() override
	{
		if (!SampleApplication::StartUp())
			return false;

		if (!m_cb.CreateConstantBuffer(m_graphics, sizeof(ModelConstants), true,
			nullptr))
			return false;

		if (!m_material.Load(m_graphics, L"Shaders/tutorial6.hlsl",
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

		m_depth = m_graphics.CreateDepthBuffer(m_window.GetScreenWidth(),
			m_window.GetScreenHeight(), DXGI_FORMAT_D24_UNORM_S8_UINT);
		if (m_depth == nullptr)
			return false;

		m_dsv = m_graphics.CreateDepthStencilView(m_depth);
		if (m_dsv == nullptr)
			return false;

		D3D11_DEPTH_STENCIL_DESC dsdesc = {};
		dsdesc.DepthEnable = true;
		dsdesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsdesc.StencilEnable = false;

		m_dss = m_graphics.CreateDepthStencilState(dsdesc);
		if (m_dss == nullptr)
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
		m_rtState.SetDepthTarget(m_dsv);
		m_rtState.SetClearColor(true, 0.0f, 0.0f, 0.0f, 1.0f);
		m_rtState.SetClearDepthStencil(true, 1.0f);
		m_rtState.SetSize(m_window.GetScreenWidth(), m_window.GetScreenHeight());

		m_timer.Start();

		m_gamepad.StartUp();

		m_keyboard.StartUp(m_window);

		m_mouse.StartUp(m_window);

		m_entityManager.StartUp(4);
		m_transformSystem.StartUp(4);
		m_rotatorSystem.StartUp(4);
		m_rotatorSystem.AddSystemRefs(&m_transformSystem);
		m_cameraSystem.StartUp(1);
		m_cameraSystem.AddSystemRefs(&m_transformSystem, &m_window);

		// entity 1
		Entity e = m_entityManager.CreateEntity();
		U64 transformHandle = m_transformSystem.CreateComponent(e);
		m_transformSystem.GetComponentByHandle(transformHandle)->transform *= DirectX::XMMatrixTranslation(-3, 0, 0);
		U64 rotatorHandle = m_rotatorSystem.CreateComponent(e);
		RotatorComponent* rotator = m_rotatorSystem.GetComponentByHandle(rotatorHandle);
		rotator->transform = transformHandle;
		rotator->speed = -1;

		// entity 2
		e = m_entityManager.CreateEntity();
		transformHandle = m_transformSystem.CreateComponent(e);
		m_transformSystem.GetComponentByHandle(transformHandle)->transform *= DirectX::XMMatrixTranslation(3, 0, 0);
		rotatorHandle = m_rotatorSystem.CreateComponent(e);
		rotator = m_rotatorSystem.GetComponentByHandle(rotatorHandle);
		rotator->transform = transformHandle;
		rotator->speed = 1;

		// entity 3 (no rotation)
		e = m_entityManager.CreateEntity();
		transformHandle = m_transformSystem.CreateComponent(e);
		m_transformSystem.GetComponentByHandle(transformHandle)->transform *= DirectX::XMMatrixTranslation(0, 0, 0);

		// camera
		e = m_entityManager.CreateEntity();
		transformHandle = m_transformSystem.CreateComponent(e);
		m_transformSystem.GetComponentByHandle(transformHandle)->transform *= DirectX::XMMatrixTranslation(0.0f, 0.0f, -5.0f);
		U64 cameraHandle = m_cameraSystem.CreateComponent(e);
		CameraComponent* camera = m_cameraSystem.GetComponentByHandle(cameraHandle);
		camera->transform = transformHandle;
		camera->nearZ = 0.01f;
		camera->farZ = 1000.0f;
		camera->fov = 45.0f;
		rotatorHandle = m_rotatorSystem.CreateComponent(e);
		rotator = m_rotatorSystem.GetComponentByHandle(rotatorHandle);
		rotator->transform = transformHandle;
		rotator->speed = 0;

		return true;
	}

	virtual void ShutDown() override
	{
		SampleApplication::ShutDown();

		m_gamepad.ShutDown();
		m_keyboard.ShutDown();
		m_mouse.ShutDown();
	}

	virtual void Update() override
	{
		m_timer.Update();
		float dt = m_timer.GetDeltaTime();
		m_time += 1.0f / 60.0f;
		
		m_gamepad.Update();
		m_keyboard.Update();
		m_mouse.Update();
		m_rotatorSystem.Execute(dt);
		m_transformSystem.Execute(dt);
		m_cameraSystem.Execute(dt);
	}

	virtual void Render() override
	{
		// update our constants with data for this frame
		ModelConstants consts;
		consts.m_viewproj = m_cameraSystem[0]->viewProjMatrix;
		consts.m_lightDirection = XMVector3Normalize(XMVectorSet(1.0f, 1.0f, -1.0f, 0.0f));
		consts.m_lightColor = XMVectorSet(0.8f, 0.8f, 0.5f, 1.0f);
		consts.m_ambientColor = XMVectorSet(0.1f, 0.1f, 0.2f, 1.0f);
		consts.m_cameraPos = m_transformSystem.GetComponentByHandle(m_cameraSystem[0]->transform)->transform.r[3];
		consts.m_specularColor = XMVectorSet(0.5f, 0.5f, 0.5f, 5.0f);

		m_graphics.SetDepthStencilState(m_dss);

		m_rtState.Begin(m_graphics);
		m_model.Select(m_graphics);

		for (int i = 0; i < m_transformSystem.Size() - 1; ++i) // exclude camera for now
		{
			TransformComponent* t = m_transformSystem[i];
			consts.m_world = t->transform;
			m_cb.MapAndSet(m_graphics, consts);
			m_material.Select(m_graphics);
			m_model.Draw(m_graphics);
		}

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
	ID3D11Texture2D* m_depth = nullptr;
	ID3D11DepthStencilView* m_dsv = nullptr;
	ID3D11DepthStencilState* m_dss = nullptr;
	float m_time = 0.0f;
	Timer m_timer;
	Gamepad m_gamepad;
	Keyboard m_keyboard;
	Mouse m_mouse;
	EntityManager m_entityManager;
	TransformSystem m_transformSystem;
	RotatorSystem m_rotatorSystem;
	CameraSystem m_cameraSystem;
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
