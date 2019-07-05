#pragma once

#include "TransformSystem.h"
#include "VelocitySystem.h"
#include "SpawnSystem.h"
#include "Types.h"
#include "EventBus.h"
#include "AppEvents.h"
#include <unordered_set>


struct DeathComponent
{
	U64 hTransform;
	U64 hVelocity;
	U32 deathCount;
};


class DeathSystem : public ComponentSystem<DeathComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, EventBus& eventBus, TransformSystem& transform, VelocitySystem& velocity, SpawnSystem& spawn)
	{
		Parent::StartUp(numComponents, em);

		eventBus.Subscribe(this, &DeathSystem::OnDeath);
		m_transformSystem = &transform;
		m_velocitySystem = &velocity;
		m_spawnSystem = &spawn;

		return true;
	}

	U64 CreateComponent(Entity e, U64 hTransform, U64 hVelocity)
	{
		U64 handle = Parent::CreateComponent(e);
		DeathComponent* comp = GetComponentByHandle(handle);

		comp->hTransform = hTransform;
		comp->hVelocity = hVelocity;
		comp->deathCount = 0;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
	}

	inline void EndFrame()
	{
		// respawn entities at end of the frame
		for (auto comp : m_condemned)
		{
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->hTransform);
			VelocityComponent* velocity = m_velocitySystem->GetComponentByHandle(comp->hVelocity);
			const SpawnComponent* spawn = m_spawnSystem->GetActiveSpawn();

			transform->position = spawn->position;
			transform->rotation = spawn->rotation;
			velocity->velocity = Vector3(0);

			comp->deathCount++;
		}
		m_condemned.clear();
	}

protected:
	void OnDeath(OnDeathEvent* deathInfo)
	{
		DeathComponent* comp = FindComponent(deathInfo->deceased);
		if (comp);
		{
			m_condemned.insert(comp);
		}		
	}

protected:
	TransformSystem* m_transformSystem;
	VelocitySystem* m_velocitySystem;
	SpawnSystem* m_spawnSystem;
	std::unordered_set<DeathComponent*> m_condemned;
};