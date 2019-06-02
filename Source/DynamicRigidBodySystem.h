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
	void AddSystemRefs(TransformSystem* transformSystem, RigidBodySystem* rigidBodySystem)
	{
		m_transformSystem = transformSystem;
		m_rigidBodySystem = rigidBodySystem;
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