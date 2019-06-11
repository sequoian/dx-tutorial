#pragma once

#include "SampleApplication.h"
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

class TestBed : public SampleApplication
{
public:

	virtual ~TestBed()
	{
		ShutDown();
	}

	virtual bool StartUp() override
	{
		if (!SampleApplication::StartUp())
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
		U64 transformHandle;
		TransformComponent* transform;
		U64 rotatorHandle;
		RotatorComponent* rotator;
		U64 cameraHandle;
		CameraComponent* camera;
		MeshComponent* mesh;
		FlyCamComponent* flycam;
		U64 colliderHandle;
		btCollisionShape* collider;
		U64 rbHandle;
		DynamicRigidBodyComponent* dynamicRB;
		U64 bulletHandle;
		RBGunComponent* rbGun;
		KinematicRigidBodyComponent* kinematicRB;
		RigidBody rb;
		U64 velocityHandle;
		KinematicCharacterControllerComponent* kinematicCC;
		U64 rigidBodyHandle;
		RigidBodyComponent* rigidBody;
		DoorComponent* door;
		DoorTriggerComponent* doorTrigger;

		// bowl
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, vec3(0, -15, 0), vec3(0), vec3(10, 1, 10));
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, vec3(0, -9, 10), vec3(90.0_rad, 0, 0), vec3(10, 1, 5));
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, vec3(0, -9, -10), vec3(90.0_rad, 0, 0), vec3(10, 1, 5));
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, vec3(10, -9, 0), vec3(0, 0, 90.0_rad), vec3(5, 1, 10));
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, vec3(-10, -9, 0), vec3(0, 0, 90.0_rad), vec3(5, 1, 10));

		// tower
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, vec3(0, 0, 25), vec3(0), vec3(10, 1, 10));

		int x = 0;
		int y = 2;
		int z = 25;
		for (int i = 0; i < 10; ++i)
		{
			m_primFactory.CreatePrimitive(PRIM_CUBE, 1, matStone, vec3(x, y, z));
			m_primFactory.CreatePrimitive(PRIM_CUBE, 1, matStone, vec3(x, y, z + 2));
			m_primFactory.CreatePrimitive(PRIM_CUBE, 1, matStone, vec3(x, y, z - 2));
			m_primFactory.CreatePrimitive(PRIM_CUBE, 1, matStone, vec3(x + 2, y, z));
			m_primFactory.CreatePrimitive(PRIM_CUBE, 1, matStone, vec3(x - 2, y, z));
			y += 2;
		}

		// trigger
		e = m_entityManager.CreateEntity();
		transformHandle = m_transformSystem.CreateComponent(e);
		transform = m_transformSystem.GetComponentByHandle(transformHandle);
		transform->position = XMVectorSet(-25, 10, 0, 1);
		transform->scale = XMVectorSet(3, 3, 3, 1);
		mesh = m_meshSystem.GetComponentByHandle(m_meshSystem.CreateComponent(e));
		mesh->transform = transformHandle;
		mesh->model = modelCube;
		mesh->material = matStone;
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider->setLocalScaling(Physics::VecFromDX(transform->scale));
		rb = m_physics.CreateStaticRigidBody(e, collider, transform->position, transform->rotation, true);
		rb.SetPosition(transform->position);
		rb.SetRotation(transform->rotation);
		rigidBody = m_rigidBodySystem.GetComponentByHandle(m_rigidBodySystem.CreateComponent(e));
		rigidBody->body = rb;
		doorTrigger = m_doorTriggerSystem.GetComponentByHandle(m_doorTriggerSystem.CreateComponent(e));

		// door
		e = m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, vec3(-25, 10, 25), vec3(90.0_rad, 0, 0), vec3(5, 1, 5), vec3(0), true);
		door = m_doorSystem.GetComponentByHandle(m_doorSystem.CreateComponent(e));
		door->startingPosition = XMVectorSet(-25, 10, 25, 1);
		door->endingPosition = XMVectorSet(-25, 20, 25, 1);
		door->secondsToMove = 3;
		m_transformSystem.GetComponentHandle(e, door->transform);
		// add door to door trigger
		doorTrigger->door = e;
		
		// walls
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, vec3(-35, 10, 25), vec3(90.0_rad, 0, 0), vec3(5, 1, 5));
		m_primFactory.CreatePrimitive(PRIM_CUBE, 0, matSand, vec3(-15, 10, 25), vec3(90.0_rad, 0, 0), vec3(5, 1, 5));

		

		// spinner
		e = m_entityManager.CreateEntity();
		transformHandle = m_transformSystem.CreateComponent(e);
		transform = m_transformSystem.GetComponentByHandle(transformHandle);
		transform->position = XMVectorSet(25, 10, 0, 1);
		transform->scale = XMVectorSet(1, 8, 8, 1);
		mesh = m_meshSystem.GetComponentByHandle(m_meshSystem.CreateComponent(e));
		mesh->transform = transformHandle;
		mesh->model = modelCube;
		mesh->material = matStone;
		rotator = m_rotatorSystem.GetComponentByHandle(m_rotatorSystem.CreateComponent(e));
		rotator->angle = 1;
		rotator->speed = 2;
		rotator->transform = transformHandle;
		collider = m_physics.CreateCollisionBox(1, 1, 1);
		collider->setLocalScaling(Physics::VecFromDX(transform->scale));
		rb = m_physics.CreateKinematicRigidBody(e, collider, transform->position, transform->rotation);
		rigidBodyHandle = m_rigidBodySystem.CreateComponent(e);
		rigidBody = m_rigidBodySystem.GetComponentByHandle(rigidBodyHandle);
		rigidBody->body = rb;
		kinematicRB = m_kinematicRBSystem.GetComponentByHandle(m_kinematicRBSystem.CreateComponent(e));
		kinematicRB->transform = transformHandle;
		kinematicRB->rigidBody = rigidBodyHandle;

		// camera
		e = m_entityManager.CreateEntity();
		transformHandle = m_transformSystem.CreateComponent(e);
		transform = m_transformSystem.GetComponentByHandle(transformHandle);
		transform->position = XMVectorSet(0, 10, -25, 1);
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
		bulletHandle = m_rbGunSystem.CreateComponent(e);
		rbGun = m_rbGunSystem.GetComponentByHandle(bulletHandle);
		rbGun->material = matStone;
		rbGun->transform = transformHandle;
		rbGun->cooldown = 0.25;
		collider = m_physics.CreateCollisionSphere(1);
		rb = m_physics.CreateCharacterBody(e, collider, transform->position, transform->rotation);
		rigidBodyHandle = m_rigidBodySystem.CreateComponent(e);
		rigidBody = m_rigidBodySystem.GetComponentByHandle(rigidBodyHandle);
		rigidBody->body = rb;
		kinematicRB = m_kinematicRBSystem.GetComponentByHandle(m_kinematicRBSystem.CreateComponent(e));
		kinematicRB->transform = transformHandle;
		kinematicRB->rigidBody = rigidBodyHandle;
		kinematicCC = m_kinematicCCSystem.GetComponentByHandle(m_kinematicCCSystem.CreateComponent(e));
		kinematicCC->transform = transformHandle;

		return true;
	}

	virtual void ShutDown() override
	{
		SampleApplication::ShutDown();

		if (m_dss != nullptr)
			m_dss->Release();
		if (m_dsv != nullptr)
			m_dsv->Release();
		if (m_depth != nullptr)
			m_depth->Release();
	}

	virtual void Update() override
	{
		SampleApplication::Update();

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
		SampleApplication::Render();

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