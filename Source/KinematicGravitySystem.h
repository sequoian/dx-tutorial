#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "MathUtility.h"

struct KinematicGravityComponent
{
	U64 hTransform;
	float gravity;
};

class KinematicGravitySystem : public ComponentSystem<KinematicGravityComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem)
	{
		Parent::StartUp(numComponents, em);
		m_transformSystem = &transformSystem;
		return true;
	}

	U64 CreateComponent(Entity e, U64 hTransform, float gravity)
	{
		U64 handle = Parent::CreateComponent(e);
		KinematicGravityComponent* comp = GetComponentByHandle(handle);

		comp->hTransform = hTransform;
		comp->gravity = gravity;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			KinematicGravityComponent* comp = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->hTransform);

			transform->position.m128_f32[1] -= comp->gravity * deltaTime;
		}
	}

protected:
	TransformSystem * m_transformSystem;
};