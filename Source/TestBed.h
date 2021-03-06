#pragma once

#include "Application.h"
#include "PrimitiveFactory.h"
#include "StringId.h"
#include "MathUtility.h"

// rendering
#include "RenderTargetState.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexFormat.h"

// component systems
#include "TransformSystem.h"
#include "RotatorSystem.h"
#include "CameraSystem.h"
#include "MeshSystem.h"
#include "FlyCamSystem.h"
#include "DynamicRigidBodySystem.h"
#include "RBGunSystem.h"
#include "KinematicRigidBodySystem.h"
#include "RBBulletSystem.h"
#include "KinematicCharacterControllerSystem.h"
#include "RigidBodySystem.h"
#include "YDespawnSystem.h"
#include "DoorSystem.h"
#include "DoorTriggerSystem.h"

// resourcess
#include "Texture.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Material.h"
#include "Model.h"

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

class TestBed : public Application
{
public:

	virtual ~TestBed()
	{
		ShutDown();
	}

	virtual bool StartUp() override
	{
		if (!Application::StartUp())
			return false;

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
		Model* modelCylinder;
		Model* modelCone;

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

		if (!m_resourceManager.LoadModel("Assets/sphere.obj", handle, "Assets/sphere.obj"_sid))
		{
			return false;
		}

		modelSphere = static_cast<Model*>(m_resourceManager.GetResourceByHandle(handle));

		if (!m_resourceManager.LoadModel("Assets/cylinder.obj", handle, "Assets/cylinder.obj"_sid))
		{
			return false;
		}

		modelCylinder = static_cast<Model*>(m_resourceManager.GetResourceByHandle(handle));

		if (!m_resourceManager.LoadModel("Assets/cone.obj", handle, "Assets/cone.obj"_sid))
		{
			return false;
		}

		modelCone = static_cast<Model*>(m_resourceManager.GetResourceByHandle(handle));

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

		m_primFactory.SetUp(&m_entityManager, &m_transformSystem, &m_meshSystem, &m_rigidBodySystem, &m_dynamicRBSystem, &m_kinematicRBSystem, &m_resourceManager, &m_physics, &m_yDespawnSystem);

		//  Init Component System

		m_transformSystem.StartUp(50, m_entityManager);
		m_rotatorSystem.StartUp(1, m_entityManager, m_transformSystem);
		m_cameraSystem.StartUp(1, m_entityManager, m_transformSystem, m_window);
		m_meshSystem.StartUp(3, m_entityManager);
		m_flycamSystem.StartUp(3, m_entityManager, m_inputManager, m_transformSystem);
		m_rigidBodySystem.StartUp(3, m_entityManager, m_physics);
		m_dynamicRBSystem.StartUp(3, m_entityManager, m_transformSystem, m_rigidBodySystem);
		m_yDespawnSystem.StartUp(20, m_entityManager, m_transformSystem);
		m_rbGunSystem.StartUp(1, m_entityManager, m_transformSystem, m_primFactory, m_inputManager,
			                  m_eventBus, m_rbBulletSystem);
		m_kinematicRBSystem.StartUp(1, m_entityManager, m_transformSystem, m_rigidBodySystem);
		m_rbBulletSystem.StartUp(1, m_entityManager, m_eventBus);
		m_kinematicCCSystem.StartUp(1, m_entityManager, m_transformSystem, m_eventBus);
		m_doorSystem.StartUp(1, m_entityManager, m_transformSystem, m_eventBus);
		m_doorTriggerSystem.StartUp(1, m_entityManager, m_eventBus);


		// Create Entities

		Entity e;
		U64 hTransform;
		U64 hRigidBody;
		RigidBody rb;
		TransformComponent* transform;
		ColliderPtr collider;

		// bowl
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, Vector3(0, -15, 0), Quaternion(), Vector3(10, 1, 10));
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, Vector3(0, -9, 10), Quaternion(90.0_rad, 0, 0), Vector3(10, 1, 5));
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, Vector3(0, -9, -10), Quaternion(90.0_rad, 0, 0), Vector3(10, 1, 5));
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, Vector3(10, -9, 0), Quaternion(0, 0, 90.0_rad), Vector3(5, 1, 10));
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, Vector3(-10, -9, 0), Quaternion(0, 0, 90.0_rad), Vector3(5, 1, 10));

		// tower
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, Vector3(0, 0, 25), Quaternion(), Vector3(10, 1, 10));

		int x = 0;
		int y = 2;
		int z = 25;
		for (int i = 0; i < 10; ++i)
		{
			m_primFactory.CreatePrimitive(PRIM_CUBE, 1, matStone, Vector3(x, y, z));
			m_primFactory.CreatePrimitive(PRIM_CUBE, 1, matStone, Vector3(x, y, z + 2));
			m_primFactory.CreatePrimitive(PRIM_CUBE, 1, matStone, Vector3(x, y, z - 2));
			m_primFactory.CreatePrimitive(PRIM_CUBE, 1, matStone, Vector3(x + 2, y, z));
			m_primFactory.CreatePrimitive(PRIM_CUBE, 1, matStone, Vector3(x - 2, y, z));
			y += 2;
		}

		// door
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(-25, 10, 25), Quaternion(90.0_rad, 0, 0), Vector3(5, 1, 5));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCube, matSand);
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider.SetScale(transform->scale);
		rb = m_physics.CreateKinematicRigidBody(e, collider, transform->position, transform->rotation);
		hRigidBody = m_rigidBodySystem.CreateComponent(e, rb);
		m_kinematicRBSystem.CreateComponent(e, hTransform, hRigidBody);
		m_doorSystem.CreateComponent(e, hTransform, XMVectorSet(-25, 10, 25, 1), XMVectorSet(-25, 20, 25, 1), 3);
		Entity doorEntity = e;

		// trigger
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, XMVectorSet(-25, 10, 0, 1), XMQuaternionIdentity(), 
															XMVectorSet(3, 3, 3, 1));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCube, matStone);
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider.SetScale(transform->scale);
		rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, transform->rotation, true);
		m_rigidBodySystem.CreateComponent(e, rb);
		m_doorTriggerSystem.CreateComponent(e, doorEntity);
		
		// walls surrounding door
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, Vector3(-35, 10, 25), Quaternion(90.0_rad, 0, 0), Vector3(5, 1, 5));
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, Vector3(-15, 10, 25), Quaternion(90.0_rad, 0, 0), Vector3(5, 1, 5));

		// spinner
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, XMVectorSet(25, 10, 0, 1), XMQuaternionIdentity(), 
															XMVectorSet(1, 8, 8, 1));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCube, matStone);
		m_rotatorSystem.CreateComponent(e, hTransform, 2, XMQuaternionIdentity());
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider.SetScale(transform->scale);
		rb = m_physics.CreateKinematicRigidBody(e, collider, transform->position, transform->rotation);
		hRigidBody = m_rigidBodySystem.CreateComponent(e, rb);
		m_kinematicRBSystem.CreateComponent(e, hTransform, hRigidBody);

		// player
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, XMVectorSet(0, 10, -25, 1));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_cameraSystem.CreateComponent(e, hTransform, 0.01f, 1000.0f, 45.0f);
		m_flycamSystem.CreateComponent(e, hTransform, 8, 18, 2, 2);
		m_rbGunSystem.CreateComponent(e, hTransform, matStone, 0.25);
		collider = m_physics.CreateCollisionSphere(1);
		rb = m_physics.CreateCharacterBody(e, collider, transform->position, transform->rotation);
		hRigidBody = m_rigidBodySystem.CreateComponent(e, rb);
		m_kinematicRBSystem.CreateComponent(e, hTransform, hRigidBody);
		m_kinematicCCSystem.CreateComponent(e, hTransform);

		return true;
	}

	virtual void ShutDown() override
	{
		Application::ShutDown();

		if (m_dss != nullptr)
			m_dss->Release();
		if (m_dsv != nullptr)
			m_dsv->Release();
		if (m_depth != nullptr)
			m_depth->Release();
	}

	virtual void Update() override
	{
		Application::Update();

		float dt = m_timer.GetDeltaTime();

		// update systems
		m_rotatorSystem.Execute(dt);
		m_transformSystem.Execute(dt);
		m_cameraSystem.Execute(dt);
		m_flycamSystem.Execute(dt);
		m_rbGunSystem.Execute(dt);
		m_doorSystem.Execute(dt);
		m_kinematicRBSystem.Execute(dt);
		m_physics.RunSimulation(dt);
		m_dynamicRBSystem.Execute(dt);
		m_yDespawnSystem.Execute(dt);
		m_transformSystem.Execute(dt);

		// end frame
		m_entityManager.EndFrame();
	}

	virtual void Render() override
	{
		Application::Render();

		// update our constants with data for this frame
		ModelConstants consts;
		consts.m_viewproj = m_cameraSystem[0]->viewProjMatrix;
		consts.m_lightDirection = XMVector3Normalize(XMVectorSet(1.0f, 1.0f, -1.0f, 0.0f));
		consts.m_lightColor = XMVectorSet(0.8f, 0.8f, 0.5f, 1.0f);
		consts.m_ambientColor = XMVectorSet(0.1f, 0.1f, 0.2f, 1.0f);
		consts.m_cameraPos = m_transformSystem.GetComponentByHandle(m_cameraSystem[0]->transform)->world.r[3];
		consts.m_specularColor = XMVectorSet(0.5f, 0.5f, 0.5f, 5.0f);

		m_graphics.SetDepthStencilState(m_dss);

		m_rtState.Begin(m_graphics);

		for (int i = 0; i < m_meshSystem.Size(); ++i)
		{
			MeshComponent* mesh = m_meshSystem[i];
			consts.m_world = m_transformSystem.GetComponentByHandle(mesh->transform)->world;
			m_cb.MapAndSet(m_graphics, consts);

			mesh->model->Select(m_graphics);
			mesh->material->Select(m_graphics);
			mesh->model->Draw(m_graphics);
		}

		m_rtState.End(m_graphics);
	}

private:
	// rendering instances
	RenderTargetState m_rtState;
	ID3D11Texture2D* m_depth = nullptr;
	ID3D11DepthStencilView* m_dsv = nullptr;
	ID3D11DepthStencilState* m_dss = nullptr;
	Buffer m_cb;

	// component systems
	TransformSystem m_transformSystem;
	RotatorSystem m_rotatorSystem;
	CameraSystem m_cameraSystem;
	MeshSystem m_meshSystem;
	FlyCamSystem m_flycamSystem;
	DynamicRigidBodySystem m_dynamicRBSystem;
	RBGunSystem m_rbGunSystem;
	KinematicRigidBodySystem m_kinematicRBSystem;
	RBBulletSystem m_rbBulletSystem;
	KinematicCharacterControllerSystem m_kinematicCCSystem;
	RigidBodySystem m_rigidBodySystem;
	YDespawnSystem m_yDespawnSystem;
	DoorSystem m_doorSystem;
	DoorTriggerSystem m_doorTriggerSystem;

	// other
	PrimitiveFactory m_primFactory;
};