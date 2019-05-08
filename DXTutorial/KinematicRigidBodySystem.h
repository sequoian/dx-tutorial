#pragma once

#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "Physics.h"
#include "Types.h"
#include "RigidBody.h"

struct KinematicRigidBodyComponent
{
	U64 transform = 0;
	RigidBody body = nullptr;
};


class KinematicRigidBodySystem : public ComponentSystem<KinematicRigidBodyComponent>
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
			KinematicRigidBodyComponent* rb = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(rb->transform);

			rb->body.SetPosition(transform->position);
			rb->body.SetRotation(transform->rotation);
		}
	}

private:
	TransformSystem * m_transformSystem;
};