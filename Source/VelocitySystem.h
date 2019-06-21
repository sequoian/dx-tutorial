#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "WriteLog.h"
#include "MathUtility.h"


struct VelocityComponent
{
	U64 hTransform;
	XMVECTOR velocity;
};


class VelocitySystem : public ComponentSystem<VelocityComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem, Physics& physics)
	{
		Parent::StartUp(numComponents, em);
		m_transformSystem = &transformSystem;
		m_physics = &physics;
		return true;
	}

	U64 CreateComponent(Entity e, U64 hTransform)
	{
		U64 handle = Parent::CreateComponent(e);
		VelocityComponent* comp = GetComponentByHandle(handle);

		comp->hTransform = hTransform;
		comp->velocity = Vector3(0);

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			VelocityComponent* comp = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->hTransform);

			transform->position += comp->velocity;
		}
	}

protected:
	TransformSystem* m_transformSystem;
	Physics* m_physics;
};