#include "SampleApplication.h"
#include "RenderTargetState.h"
#include "Material.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Model.h"
#include "VertexFormat.h"
#include "Timer.h"
#include "InputManager.h"
#include "EntityManager.h"
#include "StringId.h"

#include "TransformSystem.h"
#include "RotatorSystem.h"
#include "CameraSystem.h"
#include "MeshSystem.h"
#include "FlyCamSystem.h"

#include "ResourceManager.h"
#include "Texture.h"
#include "VertexShader.h"
#include "PixelShader.h"

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
		if (m_dss != nullptr)
			m_dss->Release();
		if (m_dsv != nullptr)
			m_dsv->Release();
		if (m_depth != nullptr)
			m_depth->Release();

		ShutDown();
	}

	virtual bool StartUp() override
	{
		if (!SampleApplication::StartUp())
			return false;

		if (!m_cb.CreateConstantBuffer(m_graphics, sizeof(ModelConstants), true,
			nullptr))
			return false;

		// Load shaders

		if (!m_vshader.Load(m_graphics, L"Shaders/tutorial6.hlsl", VertPosNormUVColor::GetVertexFormat()))
			return false;

		if (!m_pshader.Load(m_graphics, L"Shaders/tutorial6.hlsl"))
			return false;

		if (!m_model.LoadFromOBJ(m_graphics, "Assets/monkey.obj"))
			return false;

		if (!m_tex.Load(m_graphics, L"Assets/stone.tga"))
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

		m_material.SetShaders(&m_vshader, &m_pshader);
		m_material.SetConstantBuffer(0, m_cb);
		m_material.AddTexture(m_tex);
		m_material.AddShaderSampler(m_graphics.GetLinearWrapSampler());

		// create second model

		if (!m_model2.LoadFromOBJ(m_graphics, "Assets/cube.obj"))
			return false;

		// create third model
		if (!m_model3.LoadFromOBJ(m_graphics, "Assets/sphere.obj"))
			return false;

		// create second material

		m_tex2.Load(m_graphics, L"Assets/seafloor.tga");

		m_material2.SetShaders(&m_vshader, &m_pshader);
		m_material2.SetConstantBuffer(0, m_cb);
		m_material2.AddTexture(m_tex2);
		m_material2.AddShaderSampler(m_graphics.GetLinearWrapSampler());

		m_rtState.SetRenderTarget(m_window.GetRenderTarget());
		m_rtState.SetDepthTarget(m_dsv);
		m_rtState.SetClearColor(true, 0.0f, 0.0f, 0.0f, 1.0f);
		m_rtState.SetClearDepthStencil(true, 1.0f);
		m_rtState.SetSize(m_window.GetScreenWidth(), m_window.GetScreenHeight());

		m_timer.Start();

		m_inputManager.StartUp(m_window);

		m_entityManager.StartUp(4);
		m_transformSystem.StartUp(4);
		m_rotatorSystem.StartUp(4);
		m_rotatorSystem.AddSystemRefs(&m_transformSystem);
		m_cameraSystem.StartUp(1);
		m_cameraSystem.AddSystemRefs(&m_transformSystem, &m_window);
		m_meshSystem.StartUp(3);
		m_flycamSystem.StartUp(3);
		m_flycamSystem.AddSystemRefs(&m_inputManager, &m_transformSystem);

		Entity e;
		U64 transformHandle;
		U64 rotatorHandle;
		RotatorComponent* rotator;
		U64 cameraHandle;
		CameraComponent* camera;
		MeshComponent* mesh;
		FlyCamComponent* flycam;

		// entity 1
		e = m_entityManager.CreateEntity();
		transformHandle = m_transformSystem.CreateComponent(e);
		m_transformSystem.GetComponentByHandle(transformHandle)->transform *= DirectX::XMMatrixTranslation(-3, 0, 0);
		rotatorHandle = m_rotatorSystem.CreateComponent(e);
		rotator = m_rotatorSystem.GetComponentByHandle(rotatorHandle);
		rotator->transform = transformHandle;
		rotator->speed = -1;
		mesh = m_meshSystem.GetComponentByHandle(m_meshSystem.CreateComponent(e));
		mesh->transform = transformHandle;
		mesh->model = &m_model3;
		mesh->material = &m_material2;

		// entity 2
		e = m_entityManager.CreateEntity();
		transformHandle = m_transformSystem.CreateComponent(e);
		m_transformSystem.GetComponentByHandle(transformHandle)->transform *= DirectX::XMMatrixTranslation(3, 0, 0);
		rotatorHandle = m_rotatorSystem.CreateComponent(e);
		rotator = m_rotatorSystem.GetComponentByHandle(rotatorHandle);
		rotator->transform = transformHandle;
		rotator->speed = 1;
		mesh = m_meshSystem.GetComponentByHandle(m_meshSystem.CreateComponent(e));
		mesh->transform = transformHandle;
		mesh->model = &m_model2;
		mesh->material = &m_material2;

		// entity 3
		e = m_entityManager.CreateEntity();
		transformHandle = m_transformSystem.CreateComponent(e);
		m_transformSystem.GetComponentByHandle(transformHandle)->transform *= DirectX::XMMatrixTranslation(0, 0, 0);
		mesh = m_meshSystem.GetComponentByHandle(m_meshSystem.CreateComponent(e));
		mesh->transform = transformHandle;
		mesh->model = &m_model;
		mesh->material = &m_material;

		// camera
		e = m_entityManager.CreateEntity();
		transformHandle = m_transformSystem.CreateComponent(e);
		m_transformSystem.GetComponentByHandle(transformHandle)->transform *= DirectX::XMMatrixTranslation(0.0f, 0.0f, -5.0f);
		cameraHandle = m_cameraSystem.CreateComponent(e);
		camera = m_cameraSystem.GetComponentByHandle(cameraHandle);
		camera->transform = transformHandle;
		camera->nearZ = 0.01f;
		camera->farZ = 1000.0f;
		camera->fov = 45.0f;
		flycam = m_flycamSystem.GetComponentByHandle(m_flycamSystem.CreateComponent(e));
		flycam->transform = transformHandle;
		flycam->lookSpeed = 2;
		flycam->moveSpeed = 8;
		flycam->sprintSpeed = 18;
		flycam->crawlSpeed = 2;

		return true;
	}

	virtual void ShutDown() override
	{
		SampleApplication::ShutDown();

		m_inputManager.ShutDown();
		m_tex.ShutDown();
		m_tex2.ShutDown();
		m_vshader.ShutDown();
		m_pshader.ShutDown();
	}

	virtual void Update() override
	{
		m_timer.Update();
		float dt = m_timer.GetDeltaTime();
		m_time += 1.0f / 60.0f;
		
		m_inputManager.UpdateAll();

		// update systems
		m_rotatorSystem.Execute(dt);
		m_transformSystem.Execute(dt);
		m_cameraSystem.Execute(dt);
		m_flycamSystem.Execute(dt);
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

		for (int i = 0; i < m_meshSystem.Size(); ++i)
		{
			MeshComponent* mesh = m_meshSystem[i];
			consts.m_world = m_transformSystem.GetComponentByHandle(mesh->transform)->transform;
			m_cb.MapAndSet(m_graphics, consts);
			mesh->model->Select(m_graphics);
			mesh->material->Select(m_graphics);
			mesh->model->Draw(m_graphics);
		}

		m_rtState.End(m_graphics);
	}

private:
	RenderTargetState m_rtState;
	Material m_material;
	Model m_model;
	Buffer m_cb;
	Texture m_tex;

	VertexShader m_vshader;
	PixelShader m_pshader;

	// second model
	Model m_model2;

	// second material
	Material m_material2;
	Texture m_tex2;

	// third model
	Model m_model3;

	ResourceManager m_resourceManager;

	ID3D11Texture2D* m_depth = nullptr;
	ID3D11DepthStencilView* m_dsv = nullptr;
	ID3D11DepthStencilState* m_dss = nullptr;
	float m_time = 0.0f;
	Timer m_timer;
	InputManager m_inputManager;
	EntityManager m_entityManager;
	TransformSystem m_transformSystem;
	RotatorSystem m_rotatorSystem;
	CameraSystem m_cameraSystem;
	MeshSystem m_meshSystem;
	FlyCamSystem m_flycamSystem;
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
