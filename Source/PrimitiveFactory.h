#pragma once

#include "EntityManager.h"
#include "TransformSystem.h"
#include "MeshSystem.h"
#include "DynamicRigidBodySystem.h"
#include "ResourceManager.h"
#include "RigidBody.h"
#include "RigidBodySystem.h"
#include "YDespawnSystem.h"

#include "Material.h"
#include <DirectXMath.h>
using namespace DirectX;

enum PrimitiveShapes
{
	PRIM_CUBE,
	PRIM_SPHERE,
	PRIM_CYLINDER,
	PRIM_CONE
};

struct vec3
{
	vec3(float a, float b, float c)
	{
		x = a;
		y = b;
		z = c;
	}

	vec3(float a)
	{
		x = a;
		y = a;
		z = a;
	}

	float x;
	float y;
	float z;
};

class PrimitiveFactory
{
public:
	bool SetUp(EntityManager* em, TransformSystem* ts, MeshSystem* ms, RigidBodySystem* rb, DynamicRigidBodySystem* rs, ResourceManager* rm, Physics* p, YDespawnSystem* yd)
	{
		m_entityManager = em;
		m_transformSystem = ts;
		m_meshSystem = ms;
		m_dynamicRigidBodySystem = rs;
		m_resourceManager = rm;
		m_physics = p;
		m_rigidBodySystem = rb;
		m_yDespawnSystem = yd;

		return true;
	}

	Entity CreatePrimitive(PrimitiveShapes shape, float mass, Material* mat, vec3 pos, vec3 rot = vec3(0), vec3 scale = vec3(1), vec3 vel = vec3(0))
	{
		XMVECTOR dxPos = XMVectorSet(pos.x, pos.y, pos.z, 1);
		XMVECTOR dxRot = XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z);
		XMVECTOR dxScale = XMVectorSet(scale.x, scale.y, scale.z, 1);


		Entity e;
		U64 transformHandle;
		TransformComponent* transform;
		MeshComponent* mesh;
		U64 colliderHandle;
		btCollisionShape* collider;
		U64 drbHandle;
		DynamicRigidBodyComponent* dynamicRigidBody;
		U64 rbHandle;
		RigidBodyComponent* rigidBody;
		YDespawnComponent* despawn;

		Model* model;
		btCollisionShape* colShape;

		switch (shape)
		{
		case PRIM_CUBE:
			model = static_cast<Model*>(m_resourceManager->FindResourceByStringId("Assets/cube.obj"_sid));
			colShape = m_physics->CreateCollisionBox(1, 1, 1);
			break;
		case PRIM_SPHERE:
			model = static_cast<Model*>(m_resourceManager->FindResourceByStringId("Assets/sphere.obj"_sid));
			colShape = m_physics->CreateCollisionSphere(1);
			break;
		case PRIM_CYLINDER:
			model = static_cast<Model*>(m_resourceManager->FindResourceByStringId("Assets/cylinder.obj"_sid));
			colShape = m_physics->CreateCollisionCylinder(1, 1, 1);
			break;
		case PRIM_CONE:
			model = static_cast<Model*>(m_resourceManager->FindResourceByStringId("Assets/cone.obj"_sid));
			colShape = m_physics->CreateCollisionCone(1, 2);
			break;
		default:
			break;
		}


		// create entity
		e = m_entityManager->CreateEntity();

		// create transform
		transformHandle = m_transformSystem->CreateComponent(e);
		transform = m_transformSystem->GetComponentByHandle(transformHandle);
		transform->position = dxPos;
		transform->rotation = dxRot;
		transform->scale = dxScale;

		// y despawn
		despawn = m_yDespawnSystem->GetComponentByHandle(m_yDespawnSystem->CreateComponent(e));
		despawn->entity = e;
		despawn->transform = transformHandle;
		despawn->yLimit = -50;
		
		// create collider
		collider = colShape;
		collider->setLocalScaling(m_physics->VecFromDX(transform->scale));

		// create rigid body
		
		if (mass > 0)
		{
			rbHandle = m_rigidBodySystem->CreateComponent(e);
			rigidBody = m_rigidBodySystem->GetComponentByHandle(rbHandle);
			rigidBody->body = m_physics->CreateDynamicRigidBody(e, collider, dxPos, dxRot);
			rigidBody->body.SetLinearVelocity(XMVectorSet(vel.x, vel.y, vel.z, 1));
			drbHandle = m_dynamicRigidBodySystem->CreateComponent(e);
			dynamicRigidBody = m_dynamicRigidBodySystem->GetComponentByHandle(drbHandle);
			dynamicRigidBody->rigidBody = rbHandle;
			dynamicRigidBody->transform = transformHandle;
		}
		else
		{
			RigidBody rb = m_physics->CreateStaticRigidBody(e, collider, dxPos, dxRot);
			rigidBody = m_rigidBodySystem->GetComponentByHandle(m_rigidBodySystem->CreateComponent(e));
			rigidBody->body = rb;
		}
		
		

		// create mesh
		mesh = m_meshSystem->GetComponentByHandle(m_meshSystem->CreateComponent(e));
		mesh->transform = transformHandle;
		mesh->model = model;
		mesh->material = mat;

		return e;
	}

private:
	EntityManager* m_entityManager;
	TransformSystem* m_transformSystem; 
	MeshSystem* m_meshSystem; 
	DynamicRigidBodySystem* m_dynamicRigidBodySystem;
	ResourceManager* m_resourceManager;
	Physics* m_physics;
	RigidBodySystem* m_rigidBodySystem;
	YDespawnSystem* m_yDespawnSystem;

};