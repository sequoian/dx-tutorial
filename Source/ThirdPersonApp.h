#pragma once

// essentials
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
#include "PistonSystem.h"

// preprocessor directives
#define SHOW_TRIGGERS false;

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

		// Add resources to app (used for factory functions)
		m_monkey = modelMonkey;
		m_cube = modelCube;
		m_sphere = modelSphere;
		m_cone = modelCone;
		m_capsule = modelCapsule;
		m_cylinder = modelCylinder;
		m_stone = matStone;
		m_sand = matSand;

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
		m_pistonSystem.StartUp(1, m_entityManager, m_transformSystem);

		// Create Entities
		Entity e;
		U64 hTransform;
		TransformComponent* transform;
		U64 hVelocity;
		ColliderPtr collider;
		RigidBody rb;
		U64 hRigidBody;
		U64 hSpawn;

		// camera
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(0, 0, -10));
		m_cameraSystem.CreateComponent(e, hTransform, 0.01f, 1000, 45);
		U64 hCamTransform = hTransform;

		// default spawn
		e = m_entityManager.CreateEntity();
		m_spawnSystem.CreateComponent(e, Vector3(40, 22, -40), Quaternion(0, 180.0_rad, 0));

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


		// intro
		MakePlatform(Vector3(0, 0, 0), Quaternion(), Vector3(15, 1, 15), m_sand);
		MakePlatform(Vector3(0, 2, 0), Quaternion(), Vector3(15, 1, 1), m_sand);
		MakePlatform(Vector3(0, 3.4, 22), Quaternion(65.0_rad, 0, 0), Vector3(5, 8, 1), m_sand);
		MakePlatform(Vector3(0, 6.7, 33.5), Quaternion(), Vector3(8, 1, 5), m_sand);
		MakePlatform(Vector3(0, 7, 50), Quaternion(), Vector3(5, 1, 5), m_sand);
		MakePlatform(Vector3(18, 7, 50), Quaternion(), Vector3(5, 1, 5), m_sand);
		MakePlatform(Vector3(-18, 7, 50), Quaternion(), Vector3(5, 1, 5), m_sand);
		MakePlatform(Vector3(-38, 7, 50), Quaternion(), Vector3(5, 1, 5), m_sand);
		MakePlatform(Vector3(-38, 8.25, 44), Quaternion(), Vector3(5, 0.25, 1), m_sand);

		// stairs
		MakePlatform(Vector3(-38, 8.75, 43), Quaternion(), Vector3(5, 0.25, 1), m_sand);
		MakePlatform(Vector3(-38, 9.25, 42), Quaternion(), Vector3(5, 0.25, 1), m_sand);
		MakePlatform(Vector3(-38, 9.75, 41), Quaternion(), Vector3(5, 0.25, 1), m_sand);
		MakePlatform(Vector3(-38, 10.25, 40), Quaternion(), Vector3(5, 0.25, 1), m_sand);
		MakePlatform(Vector3(-38, 10.75, 34.5), Quaternion(), Vector3(5, 0.25, 5), m_sand);

		// climb up
		MakePlatform(Vector3(-38, 12.5, 22), Quaternion(), Vector3(3, 0.5, 3), m_sand);
		MakePlatform(Vector3(-48, 15, 15), Quaternion(), Vector3(3, 0.5, 3), m_sand);
		MakePlatform(Vector3(-40, 18.5, 10), Quaternion(), Vector3(3, 0.5, 3), m_sand);

		// obstacle course
		MakePlatform(Vector3(-45, 20, -20), Quaternion(), Vector3(10, 0.5, 25), m_sand);

		// obstacles with jumping
		MakePlatform(Vector3(-25, 20, -40), Quaternion(), Vector3(3, 1, 5), m_sand);
		MakePlatform(Vector3(-10, 20, -40), Quaternion(), Vector3(3, 1, 5), m_sand);
		MakePlatform(Vector3(5, 20, -40), Quaternion(), Vector3(3, 1, 5), m_sand);
		MakePlatform(Vector3(20, 20, -40), Quaternion(), Vector3(3, 1, 3), m_sand);
		MakePlatform(Vector3(40, 20, -40), Quaternion(), Vector3(8, 1, 8), m_sand);
		MakePlatform(Vector3(40, 20, -24), Quaternion(), Vector3(4, 1, 8), m_sand);
		MakePlatform(Vector3(40, 20, -7), Quaternion(), Vector3(9, 1, 9), m_sand);

		// doorway
		MakePlatform(Vector3(34, 24, 2.5), Quaternion(), Vector3(3, 3, 0.5), m_sand);
		MakePlatform(Vector3(40, 30, 2.5), Quaternion(), Vector3(3, 3, 0.5), m_sand);
		MakePlatform(Vector3(46, 24, 2.5), Quaternion(), Vector3(3, 3, 0.5), m_sand);
		MakePlatform(Vector3(40, 20, 11), Quaternion(), Vector3(9, 1, 9), m_sand);
		
		/*

		// ground
		MakePlatform(Vector3(0, -15, 0), Quaternion(), Vector3(5, 1, 5), m_sand);
		// north slope
		MakePlatform(Vector3(0, -12, 7), Quaternion(-45.0_rad, 0, 0), Vector3(5, 1, 5), m_sand);
		// east slope
		MakePlatform(Vector3(7, -11, 0), Quaternion(0, 0, 65.0_rad), Vector3(5, 1, 5), m_sand);
		// west slope
		MakePlatform(Vector3(-9, -13, 0), Quaternion(0, 0, -25.0_rad), Vector3(5, 1, 5), m_sand);
		// west wall
		MakePlatform(Vector3(-14, -7, 0), Quaternion(0, 0, 90.0_rad), Vector3(5, 1, 5), m_sand);

		// island 1
		MakePlatform(Vector3(0, -15, -18), Quaternion(), Vector3(5, 1, 5), m_sand);
		MakeCheckpoint(Vector3(0, -13, -18), Quaternion(), Vector3(0, -13, -18), Quaternion(), Vector3(4, 1, 4));

		// island 2
		MakePlatform(Vector3(0, -13, -35), Quaternion(), Vector3(5, 1, 5), m_sand);
		MakeCheckpoint(Vector3(0, -11, -35), Quaternion(), Vector3(0, -11, -35), Quaternion(), Vector3(4, 1, 4));

		// coins
		MakeCoin(Vector3(0, -8, 4));
		MakeCoin(Vector3(4, -10, 0));
		MakeCoin(Vector3(-8, -5, 0));
		MakeCoin(Vector3(0, -10, -18));
		MakeCoin(Vector3(0, -8, -35));
		MakeCoin(Vector3(2, -8, -35));
		MakeCoin(Vector3(-2, -8, -35));

		// kill trigger under platforms
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(0, -25, -25), Quaternion(), Vector3(40, 1, 40));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		//m_meshSystem.CreateComponent(e, hTransform, modelCube, matStone);
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider.SetScale(transform->scale);
		rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, transform->rotation, true);
		m_rigidBodySystem.CreateComponent(e, rb);
		m_deadlyTouchSystem.CreateComponent(e);

		// obstacle
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(0, -15, -10), Quaternion(), Vector3(0.5, 10, 0.5));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCube, matStone);
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider.SetScale(transform->scale);
		rb = m_physics.CreateKinematicRigidBody(e, collider, transform->position, transform->rotation, true);
		hRigidBody = m_rigidBodySystem.CreateComponent(e, rb);
		m_kinematicRBSystem.CreateComponent(e, hTransform, hRigidBody);
		m_deadlyTouchSystem.CreateComponent(e);
		m_rotatorSystem.CreateComponent(e, hTransform, 2.5, transform->rotation, Vector3(0, 0, 1));

		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(0, -15, -10), Quaternion(), Vector3(10, 0.5, 0.5));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCube, matStone);
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider.SetScale(transform->scale);
		rb = m_physics.CreateKinematicRigidBody(e, collider, transform->position, transform->rotation, true);
		hRigidBody = m_rigidBodySystem.CreateComponent(e, rb);
		m_kinematicRBSystem.CreateComponent(e, hTransform, hRigidBody);
		m_deadlyTouchSystem.CreateComponent(e);
		m_rotatorSystem.CreateComponent(e, hTransform, 2.5, transform->rotation, Vector3(0, 0, 1));

		// piston
		e = m_entityManager.CreateEntity();
		hTransform = m_transformSystem.CreateComponent(e, Vector3(5, -13, -28), Quaternion(), Vector3(2, 5, 0.5));
		transform = m_transformSystem.GetComponentByHandle(hTransform);
		m_meshSystem.CreateComponent(e, hTransform, modelCube, matStone);
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider.SetScale(transform->scale);
		rb = m_physics.CreateKinematicRigidBody(e, collider, transform->position, transform->rotation, true);
		hRigidBody = m_rigidBodySystem.CreateComponent(e, rb);
		m_kinematicRBSystem.CreateComponent(e, hTransform, hRigidBody);
		m_deadlyTouchSystem.CreateComponent(e);
		m_pistonSystem.CreateComponent(e, hTransform, transform->position, Vector3(-5, -13, -28), 0.5, 0.5, 0.5, 0.5);
		
		*/

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
		m_pistonSystem.Execute(dt);
		
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
	PistonSystem m_pistonSystem;
	
// Cache resources for use in factory methods
private:
	Model* m_monkey;
	Model* m_cube;
	Model* m_capsule;
	Model* m_cylinder;
	Model* m_cone;
	Model* m_sphere;
	Material* m_sand;
	Material* m_stone;

// Factory method declarations
private:
	Entity MakePlatform(XMVECTOR pos, XMVECTOR rot, XMVECTOR scale, Material* material);
	Entity MakeCoin(XMVECTOR position);
	Entity MakeCheckpoint(XMVECTOR spawnPos, XMVECTOR spawnRot, XMVECTOR triggerPos, XMVECTOR triggerRot, XMVECTOR triggerScale);

};


// Factory Methods

Entity ThirdPersonApp::MakePlatform(XMVECTOR pos, XMVECTOR rot, XMVECTOR scale, Material* material)
{
	Entity e = m_entityManager.CreateEntity();
	U64 hTransform = m_transformSystem.CreateComponent(e, pos, rot, scale);
	TransformComponent* transform = m_transformSystem.GetComponentByHandle(hTransform);
	m_meshSystem.CreateComponent(e, hTransform, m_cube, material);
	ColliderPtr collider = m_physics.CreateCollisionBox(1, 1, 1);
	collider.SetScale(transform->scale);
	RigidBody rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, transform->rotation);
	m_rigidBodySystem.CreateComponent(e, rb);

	return e;
}

Entity ThirdPersonApp::MakeCoin(XMVECTOR position)
{
	Entity e = m_entityManager.CreateEntity();
	U64 hTransform = m_transformSystem.CreateComponent(e, position, Quaternion(90.0_rad, 0, 0), Vector3(0.5, 0.1, 0.5));
	TransformComponent* transform = m_transformSystem.GetComponentByHandle(hTransform);
	m_meshSystem.CreateComponent(e, hTransform, m_cylinder, m_stone);
	ColliderPtr collider = m_physics.CreateCollisionSphere(0.5);
	RigidBody rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, Quaternion(), true);
	m_rigidBodySystem.CreateComponent(e, rb);
	m_coinSystem.CreateComponent(e);
	m_rotatorSystem.CreateComponent(e, hTransform, 3, transform->rotation);

	return e;
}

Entity ThirdPersonApp::MakeCheckpoint(XMVECTOR spawnPos, XMVECTOR spawnRot, XMVECTOR triggerPos, XMVECTOR triggerRot, XMVECTOR triggerScale)
{
	Entity e = m_entityManager.CreateEntity();

	// Create spawn point
	U64 hSpawn = m_spawnSystem.CreateComponent(e, spawnPos, spawnRot);

	// Create checkpoint trigger
	U64 hTransform = m_transformSystem.CreateComponent(e, triggerPos, triggerRot, triggerScale);
	TransformComponent* transform = m_transformSystem.GetComponentByHandle(hTransform);
	#if SHOW_TRIGGERS == true
		m_meshSystem.CreateComponent(e, hTransform, m_cube, m_stone);
	#endif
	ColliderPtr collider = m_physics.CreateCollisionBox(1, 1, 1);
	collider.SetScale(transform->scale);
	RigidBody rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, transform->rotation, true);
	m_rigidBodySystem.CreateComponent(e, rb);
	m_checkpointTriggerSystem.CreateComponent(e, hSpawn);

	return e;
}
