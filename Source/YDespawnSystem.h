#pragma once

#include "TransformSystem.h"
#include "Physics.h"
#include "Types.h"
#include "RigidBody.h"
#include "EventBus.h"
#include "VelocitySystem.h"


struct YDespawnComponent
{
	Entity entity;
	U64 transform;
	I32 yLimit = -100;
};


class YDespawnSystem : public ComponentSystem<YDespawnComponent>
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
			YDespawnComponent* comp = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->transform);

			if (transform->position.m128_f32[1] < comp->yLimit)
			{
				m_entityManager->Destroy(comp->entity);
			}
		}
	}

private:
	TransformSystem * m_transformSystem;
};