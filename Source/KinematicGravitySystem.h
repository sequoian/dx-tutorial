#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "MathUtility.h"
#include "VelocitySystem.h"

struct KinematicGravityComponent
{
	U64 hTransform;
	U64 hVelocity;
	float gravity;
};

class KinematicGravitySystem : public ComponentSystem<KinematicGravityComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem, VelocitySystem& velocity)
	{
		Parent::StartUp(numComponents, em);
		m_transformSystem = &transformSystem;
		m_velocitySystem = &velocity;
		return true;
	}

	U64 CreateComponent(Entity e, U64 hTransform, U64 hVelocity, float gravity)
	{
		U64 handle = Parent::CreateComponent(e);
		KinematicGravityComponent* comp = GetComponentByHandle(handle);

		comp->hTransform = hTransform;
		comp->hVelocity = hVelocity;
		comp->gravity = gravity;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			KinematicGravityComponent* comp = m_pool[i];
			// still need transform?
			//TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->hTransform);
			VelocityComponent* velocity = m_velocitySystem->GetComponentByHandle(comp->hVelocity);

			velocity->velocity += Vector3(0, -comp->gravity * deltaTime, 0);
		}
	}

protected:
	TransformSystem* m_transformSystem;
	VelocitySystem* m_velocitySystem;
};