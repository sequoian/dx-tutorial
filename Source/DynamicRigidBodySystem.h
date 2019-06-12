#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "Physics.h"
#include "Types.h"
#include "RigidBodySystem.h"


struct DynamicRigidBodyComponent
{
	U64 transform = 0;
	U64 rigidBody = 0;
};


class DynamicRigidBodySystem : public ComponentSystem<DynamicRigidBodyComponent>
{
public:

	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem, RigidBodySystem& rigidBodySystem)
	{
		TParent::StartUp(numComponents, em);

		m_transformSystem = &transformSystem;
		m_rigidBodySystem = &rigidBodySystem;

		return true;
	}

	U64 CreateComponent(Entity e, U64 hTransform, U64 hRigidBody)
	{
		U64 handle = TParent::CreateComponent(e);
		DynamicRigidBodyComponent* comp = GetComponentByHandle(handle);

		comp->transform = hTransform;
		comp->rigidBody = hRigidBody;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			DynamicRigidBodyComponent* drb = m_pool[i];
			RigidBodyComponent* rb = m_rigidBodySystem->GetComponentByHandle(drb->rigidBody);

			TransformComponent* transform = m_transformSystem->GetComponentByHandle(drb->transform);

			transform->position = rb->body.GetPosition();
			transform->rotation = rb->body.GetRotation();
		}
	}

private:
	TransformSystem* m_transformSystem;
	RigidBodySystem* m_rigidBodySystem;
};