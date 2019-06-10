#pragma once

#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "Physics.h"
#include "Types.h"
#include "RigidBodySystem.h"

struct KinematicRigidBodyComponent
{
	U64 transform = 0;
	U64 rigidBody = 0;
};


class KinematicRigidBodySystem : public ComponentSystem<KinematicRigidBodyComponent>
{
public:

	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem, RigidBodySystem& rigidBodySystem)
	{
		ComponentSystem<KinematicRigidBodyComponent>::StartUp(numComponents, em);

		m_transformSystem = &transformSystem;
		m_rigidBodySystem = &rigidBodySystem;

		return true;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			KinematicRigidBodyComponent* krb = m_pool[i];
			RigidBodyComponent* rb = m_rigidBodySystem->GetComponentByHandle(krb->rigidBody);

			TransformComponent* transform = m_transformSystem->GetComponentByHandle(krb->transform);

			rb->body.SetPosition(transform->position);
			rb->body.SetRotation(transform->rotation);
		}
	}

private:
	TransformSystem * m_transformSystem;
	RigidBodySystem* m_rigidBodySystem;
};