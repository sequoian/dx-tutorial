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

// resourcess
#include "Texture.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Material.h"
#include "Model.h"

// component systems
#include "TransformSystem.h"
#include "MeshSystem.h"
#include "CameraSystem.h"
#include "PivotCamSystem.h"
#include "KinematicGravitySystem.h"
#include "RigidBodySystem.h"
#include "LegCastSystem.h"
#include "MovementSystem.h"
#include "VelocitySystem.h"
#include "KinematicRigidBodySystem.h"
#include "KinematicCharacterControllerSystem.h"
#include "JumpSystem.h"
#include "CoinSystem.h"
#include "RotatorSystem.h"
#include "SpawnSystem.h"
#include "DeadlyTouchSystem.h"
#include "DeathSystem.h"
#include "CheckpointTriggerSystem.h"

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

class ThirdPersonApp : public Application
{
public:

	virtual ~ThirdPersonApp()
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
		Model* modelCapsule;

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

		if (!m_resourceManager.LoadModel("Assets/capsule.obj", handle, "Assets/capsule.obj"_sid))
		{
			return false;
		}

		modelCapsule = static_cast<Model*>(m_resourceManager.GetResourceByHandle(handle));

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
		m_rtState.SetClearColor(true, 0.4f, 0.5f, 1.0f, 1.0f);
		m_rtState.SetClearDepthStencil(true, 1.0f);
		m_rtState.SetSize(m_window.GetScreenWidth(), m_window.GetScreenHeight());

		//  Init Component System
		m_transformSystem.StartUp(3, m_entityManager);
		m_cameraSystem.StartUp(1, m_entityManager, m_transformSystem, m_window);
		m_meshSystem.StartUp(2, m_entityManager);
		m_pivotCamSystem.StartUp(1, m_entityManager, m_transformSystem, m_inputManager);
		m_gravitySystem.StartUp(1, m_entityManager, m_transformSystem, m_velocitySystem, m_legCastSystem);
		m_rigidBodySystem.StartUp(2, m_entityManager, m_physics);
		m_legCastSystem.StartUp(1, m_entityManager, m_transformSystem, m_physics, m_velocitySystem);
		m_movementSystem.StartUp(1, m_entityManager, m_transformSystem, m_inputManager, m_pivotCamSystem, m_velocitySystem);
		m_velocitySystem.StartUp(1, m_entityManager, m_transformSystem, m_physics);
		m_kinematicRBSystem.StartUp(1, m_entityManager, m_transformSystem, m_rigidBodySystem);
		m_kinematicCCSystem.StartUp(1, m_entityManager, m_transformSystem, m_eventBus);
		m_jumpSystem.StartUp(1, m_entityManager, m_velocitySystem, m_legCastSystem, m_inputManager);
		m_coinSystem.StartUp(5, m_entityManager, m_eventBus);
		m_rotatorSystem.StartUp(5, m_entityManager, m_transformSystem);
		m_spawnSystem.StartUp(1, m_entityManager);
		m_deadlyTouchSystem.StartUp(1, m_entityManager, m_eventBus);
		m_deathSystem.StartUp(1, m_entityManager, m_eventBus, m_transformSystem, m_velocitySystem, m_spawnSystem);
		m_checkpointTriggerSystem.StartUp(1, m_entityManager, m_eventBus, m_spawnSystem);

		// Create Entities
		Entity e;
		U64 hTransform;
		TransformComponent* transform;
		U64 hVelocity;
		ColliderPtr collider;
		RigidBody rb;
		U64 hRigidBody;

		// camera
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(0, 0, -10));
		m_cameraSystem.CreateComponent(e, hTransform, 0.01f, 1000, 45);
		U64 hCamTransform = hTransform;

		// default spawn
		e = m_entityManager.CreateEntity();
		m_spawnSystem.CreateComponent(e, Vector3(0, -10, 0), Quaternion());

		// spawn with checkpoint
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(1, -14, -4), Quaternion(), Vector3(3, 1, 1));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCube, matStone);
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider.SetScale(transform->scale);
		rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, transform->rotation, true);
		m_rigidBodySystem.CreateComponent(e, rb);
		U64 hSpawn = m_spawnSystem.CreateComponent(e, transform->position, transform->rotation);
		m_checkpointTriggerSystem.CreateComponent(e, hSpawn);

		// player
		e = m_entityManager.CreateEntity();
		const SpawnComponent* defaultSpawn = m_spawnSystem.GetActiveSpawn();
		hTransform = m_transformSystem.CreateComponent(e, defaultSpawn->position, defaultSpawn->rotation);
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		hVelocity = m_velocitySystem.CreateComponent(e, hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCapsule, matStone);
		U64 hPivotCam = m_pivotCamSystem.CreateComponent(e, hTransform, hCamTransform, 5, 5);
		U64 hLegCast =  m_legCastSystem.CreateComponent(e, hTransform, hVelocity, 1.5);
		m_gravitySystem.CreateComponent(e, hTransform, hVelocity, hLegCast, 0.3);
		m_movementSystem.CreateComponent(e, hTransform, hPivotCam, hVelocity, 1);
		m_jumpSystem.CreateComponent(e, hVelocity, hLegCast, 0.2);
		collider = m_physics.CreateCollisionCapsule(0.5, 1);
		rb = m_physics.CreateCharacterBody(e, collider, transform->position, transform->rotation);
		hRigidBody = m_rigidBodySystem.CreateComponent(e, rb);
		m_kinematicRBSystem.CreateComponent(e, hTransform, hRigidBody);
		m_kinematicCCSystem.CreateComponent(e, hTransform);
		m_deathSystem.CreateComponent(e, hTransform, hVelocity);

		// coin 1
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(0, -8, 4), Quaternion(90.0_rad, 0, 0), Vector3(0.5, 0.1, 0.5));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCylinder, matStone);
		collider = m_physics.CreateCollisionSphere(0.5);
		rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, Quaternion(), true);
		m_coinSystem.CreateComponent(e);
		m_rotatorSystem.CreateComponent(e, hTransform, 3, transform->rotation);

		// coin 2
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(4, -10, 0), Quaternion(90.0_rad, 0, 0), Vector3(0.5, 0.1, 0.5));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCylinder, matStone);
		collider = m_physics.CreateCollisionSphere(0.5);
		rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, Quaternion(), true);
		m_coinSystem.CreateComponent(e);
		m_rotatorSystem.CreateComponent(e, hTransform, 3, transform->rotation);

		// coin 3
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(-8, -5, 0), Quaternion(90.0_rad, 0, 0), Vector3(0.5, 0.1, 0.5));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCylinder, matStone);
		collider = m_physics.CreateCollisionSphere(0.5);
		rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, Quaternion(), true);
		m_coinSystem.CreateComponent(e);
		m_rotatorSystem.CreateComponent(e, hTransform, 3, transform->rotation);

		// ground
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(0, -15, 0), Quaternion(), Vector3(5, 1, 5));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCube, matSand);
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider.SetScale(transform->scale);
		rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, transform->rotation);
		m_rigidBodySystem.CreateComponent(e, rb);

		// slope 1
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(0, -12, 7), Quaternion(-45.0_rad, 0, 0), Vector3(5, 1, 5));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCube, matSand);
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider.SetScale(transform->scale);
		rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, transform->rotation);
		m_rigidBodySystem.CreateComponent(e, rb);

		// slope 2
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(7, -11, 0), Quaternion(0, 0, 65.0_rad), Vector3(5, 1, 5));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCube, matSand);
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider.SetScale(transform->scale);
		rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, transform->rotation);
		m_rigidBodySystem.CreateComponent(e, rb);
		m_deadlyTouchSystem.CreateComponent(e);

		// slope 3
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(-7, -13, 0), Quaternion(0, 0, -25.0_rad), Vector3(5, 1, 5));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCube, matSand);
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider.SetScale(transform->scale);
		rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, transform->rotation);
		m_rigidBodySystem.CreateComponent(e, rb);

		// wall 1
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(-12, -10, 0), Quaternion(0, 0, 90.0_rad), Vector3(5, 1, 5));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCube, matSand);
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider.SetScale(transform->scale);
		rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, transform->rotation);
		m_rigidBodySystem.CreateComponent(e, rb);
			
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
		m_movementSystem.Execute(dt);
		m_jumpSystem.Execute(dt);
		m_gravitySystem.Execute(dt);
		m_velocitySystem.Execute(dt);
		m_rotatorSystem.Execute(dt);
		
		m_kinematicRBSystem.Execute(dt);
		m_physics.RunSimulation(dt);
		
		
		m_legCastSystem.Execute(dt);
		m_pivotCamSystem.Execute(dt);

		
		m_transformSystem.Execute(dt);
		m_cameraSystem.Execute(dt);

		// end frame
		m_deathSystem.EndFrame();
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
	CameraSystem m_cameraSystem;
	MeshSystem m_meshSystem;
	PivotCamSystem m_pivotCamSystem;
	KinematicGravitySystem m_gravitySystem;
	RigidBodySystem m_rigidBodySystem;
	LegCastSystem m_legCastSystem;
	MovementSystem m_movementSystem;
	VelocitySystem m_velocitySystem;
	KinematicRigidBodySystem m_kinematicRBSystem;
	KinematicCharacterControllerSystem m_kinematicCCSystem;
	JumpSystem m_jumpSystem;
	CoinSystem m_coinSystem;
	RotatorSystem m_rotatorSystem;
	SpawnSystem m_spawnSystem;
	DeadlyTouchSystem m_deadlyTouchSystem;
	DeathSystem m_deathSystem;
	CheckpointTriggerSystem m_checkpointTriggerSystem;

	// other
	PrimitiveFactory m_primFactory;
};