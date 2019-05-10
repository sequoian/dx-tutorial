#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "Physics.h"
#include "Types.h"


struct DynamicRigidBodyComponent
{
	U64 transform = 0;
	RigidBody body;
};


class DynamicRigidBodySystem : public ComponentSystem<DynamicRigidBodyComponent>
{
public:
	void AddSystemRefs(TransformSystem* transformSystem)
	{
		m_transformSystem = transformSystem;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			DynamicRigidBodyComponent* rb = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(rb->transform);

			transform->position = rb->body.GetPosition();
			transform->rotation = rb->body.GetRotation();
		}
	}

private:
	TransformSystem* m_transformSystem;
};