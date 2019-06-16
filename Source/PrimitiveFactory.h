#pragma once

#include "EntityManager.h"
#include "TransformSystem.h"
#include "MeshSystem.h"
#include "DynamicRigidBodySystem.h"
#include "ResourceManager.h"
#include "RigidBody.h"
#include "RigidBodySystem.h"
#include "YDespawnSystem.h"
#include "KinematicRigidBodySystem.h"
#include "Physics.h"
#include "ColliderPtr.h"

#include "Material.h"
#include "MathUtility.h"

enum PrimitiveShapes
{
	PRIM_CUBE,
	PRIM_SPHERE,
	PRIM_CYLINDER,
	PRIM_CONE
};

class PrimitiveFactory
{
public:
	bool SetUp(EntityManager* em, TransformSystem* ts, MeshSystem* ms, RigidBodySystem* rb, DynamicRigidBodySystem* rs, KinematicRigidBodySystem* krs, ResourceManager* rm, Physics* p, YDespawnSystem* yd)
	{
		m_entityManager = em;
		m_transformSystem = ts;
		m_meshSystem = ms;
		m_dynamicRigidBodySystem = rs;
		m_resourceManager = rm;
		m_physics = p;
		m_rigidBodySystem = rb;
		m_yDespawnSystem = yd;
		m_kinematicRigidBodySystem = krs;


		return true;
	}

	Entity CreatePrimitive(PrimitiveShapes shape, float mass, Material* mat, XMVECTOR pos, XMVECTOR rot = Vector3(0),
		                   XMVECTOR scale = Vector3(1), XMVECTOR vel = Vector3(0), bool isKinematic = false)
	{
		Entity e;
		U64 transformHandle;
		TransformComponent* transform;
		ColliderPtr collider;
		U64 rbHandle;
		RigidBody rb;
		Model* model;

		switch (shape)
		{
		case PRIM_CUBE:
			model = static_cast<Model*>(m_resourceManager->FindResourceByStringId("Assets/cube.obj"_sid));
			collider = m_physics->CreateCollisionBox(1, 1, 1);
			break;
		case PRIM_SPHERE:
			model = static_cast<Model*>(m_resourceManager->FindResourceByStringId("Assets/sphere.obj"_sid));
			collider = m_physics->CreateCollisionSphere(1);
			break;
		case PRIM_CYLINDER:
			model = static_cast<Model*>(m_resourceManager->FindResourceByStringId("Assets/cylinder.obj"_sid));
			collider = m_physics->CreateCollisionCylinder(1, 1, 1);
			break;
		case PRIM_CONE:
			model = static_cast<Model*>(m_resourceManager->FindResourceByStringId("Assets/cone.obj"_sid));
			collider = m_physics->CreateCollisionCone(1, 2);
			break;
		default:
			break;
		}


		// create entity
		e = m_entityManager->CreateEntity();

		// create transform
		transformHandle = m_transformSystem->CreateComponent(e, pos, rot, scale);
		transform = m_transformSystem->GetComponentByHandle(transformHandle);

		// y despawn
		m_yDespawnSystem->GetComponentByHandle(m_yDespawnSystem->CreateComponent(e, transformHandle, -50));
		
		// create collider
		collider.SetScale(transform->scale);

		// create rigid body
		
		if (mass > 0)
		{
			rb = m_physics->CreateDynamicRigidBody(e, collider, pos, rot);
			rb.SetLinearVelocity(vel);
			rbHandle = m_rigidBodySystem->CreateComponent(e, rb);
			m_dynamicRigidBodySystem->CreateComponent(e, transformHandle, rbHandle);
		}
		else if (isKinematic)
		{
			rb = m_physics->CreateKinematicRigidBody(e, collider, pos, rot);
			rbHandle = m_rigidBodySystem->CreateComponent(e, rb);
			m_kinematicRigidBodySystem->CreateComponent(e, transformHandle, rbHandle);

		}
		else
		{
			rb = m_physics->CreateStaticRigidBody(e, collider, pos, rot);
			m_rigidBodySystem->CreateComponent(e, rb);
		}
		
		// create mesh
		m_meshSystem->GetComponentByHandle(m_meshSystem->CreateComponent(e, transformHandle, model, mat));

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
	KinematicRigidBodySystem* m_kinematicRigidBodySystem;

};