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

#include "Physics.h"

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

		// Start systems
		m_resourceManager.StartUp(m_graphics);

		// create constant buffer
		if (!m_cb.CreateConstantBuffer(m_graphics, sizeof(ModelConstants), true,
			nullptr))
			return false;

		U64 handle;

		// Load shaders
		VertexShader* vsBase;
		PixelShader* psBase;

		if (!m_resourceManager.LoadVertexShader("Shaders/tutorial6.hlsl", handle, VertPosNormUVColor::GetVertexFormat(), "vsbase"_sid))
		{
			return false;
		}

		vsBase = static_cast<VertexShader*>(m_resourceManager.GetResourceByHandle(handle));

		if (!m_resourceManager.LoadPixelShader("Shaders/tutorial6.hlsl", handle, "psbase"_sid))
		{
			return false;
		}

		psBase = static_cast<PixelShader*>(m_resourceManager.GetResourceByHandle(handle));

		// Load models
		Model* modelMonkey;
		Model* modelCube;
		Model* modelSphere;

		if (!m_resourceManager.LoadModel("Assets/monkey.obj", handle, "Assets/monkey.obj"_sid))
		{
			return false;
		}

		modelMonkey = static_cast<Model*>(m_resourceManager.GetResourceByHandle(handle));

		if (!m_resourceManager.LoadModel("Assets/cube.obj", handle, "Assets/cube.obj"_sid))
		{
			return false;
		}

		modelCube = static_cast<Model*>(m_resourceManager.GetResourceByHandle(handle));

		if (!m_resourceManager.LoadModel("Assets/sphere.obj", handle, "Assets/sphere.obj"_sid))
		{
			return false;
		}

		modelSphere = static_cast<Model*>(m_resourceManager.GetResourceByHandle(handle));

		// Load textures
		Texture* texStone;
		Texture* texSeafloor;

		if (!m_resourceManager.LoadTexture("Assets/stone.tga", handle, "Assets/stone.tga"_sid))
		{
			return false;
		}

		texStone = static_cast<Texture*>(m_resourceManager.GetResourceByHandle(handle));

		if (!m_resourceManager.LoadTexture("Assets/seafloor.tga", handle, "Assets/seafloor.tga"_sid))
		{
			return false;
		}

		texSeafloor = static_cast<Texture*>(m_resourceManager.GetResourceByHandle(handle));

		// Load materials
		Material* matStone;
		Material* matSand;

		m_resourceManager.CreateMaterial(handle, "Stone"_sid);
		matStone = static_cast<Material*>(m_resourceManager.GetResourceByHandle(handle));
		matStone->SetShaders(vsBase, psBase);
		matStone->SetConstantBuffer(0, m_cb);
		matStone->AddTexture(*texStone);
		matStone->AddShaderSampler(m_graphics.GetLinearWrapSampler());

		m_resourceManager.CreateMaterial(handle, "Sand"_sid);
		matSand = static_cast<Material*>(m_resourceManager.GetResourceByHandle(handle));
		matSand->SetShaders(vsBase, psBase);
		matSand->SetConstantBuffer(0, m_cb);
		matSand->AddTexture(*texSeafloor);
		matSand->AddShaderSampler(m_graphics.GetLinearWrapSampler());

		// Create render targets

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


		m_rtState.SetRenderTarget(m_window.GetRenderTarget());
		m_rtState.SetDepthTarget(m_dsv);
		m_rtState.SetClearColor(true, 0.0f, 0.0f, 0.0f, 1.0f);
		m_rtState.SetClearDepthStencil(true, 1.0f);
		m_rtState.SetSize(m_window.GetScreenWidth(), m_window.GetScreenHeight());

		m_timer.Start();
		m_inputManager.StartUp(m_window);

		// Create entities

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
		mesh->model = modelSphere;
		mesh->material = matStone;

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
		mesh->model = modelCube;
		mesh->material = matStone;

		// entity 3
		e = m_entityManager.CreateEntity();
		transformHandle = m_transformSystem.CreateComponent(e);
		m_transformSystem.GetComponentByHandle(transformHandle)->transform *= DirectX::XMMatrixTranslation(0, 0, 0);
		mesh = m_meshSystem.GetComponentByHandle(m_meshSystem.CreateComponent(e));
		mesh->transform = transformHandle;
		mesh->model = modelMonkey;
		mesh->material = matStone;

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

		// test
		Physics::Test();

		return true;
	}

	virtual void ShutDown() override
	{
		SampleApplication::ShutDown();

		m_inputManager.ShutDown();
	}

	virtual void Update() override
	{
		m_timer.Update();
		float dt = m_timer.GetDeltaTime();
		
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
	// rendering
	RenderTargetState m_rtState;
	ID3D11Texture2D* m_depth = nullptr;
	ID3D11DepthStencilView* m_dsv = nullptr;
	ID3D11DepthStencilState* m_dss = nullptr;

	Buffer m_cb;
	
	// systems
	Timer m_timer;
	ResourceManager m_resourceManager;
	InputManager m_inputManager;
	EntityManager m_entityManager;

	// component systems
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
