#pragma once

#include "ComponentSystem.h"
#include "SpawnSystem.h"
#include "EventBus.h"
#include "AppEvents.h"

struct CheckpointTriggerComponent
{
	U64 hSpawn;
};


class CheckpointTriggerSystem : public ComponentSystem<CheckpointTriggerComponent>
{
public:

	bool StartUp(U32 numComponents, EntityManager& em, EventBus& bus, SpawnSystem& spawn)
	{
		Parent::StartUp(numComponents, em);

		SubscribeToCollisionEvents(bus);
		m_spawnSystem = &spawn;

		return true;
	}

	U64 CreateComponent(Entity e, U64 hSpawn)
	{
		U64 handle = Parent::CreateComponent(e);
		CheckpointTriggerComponent* comp = GetComponentByHandle(handle);

		comp->hSpawn = hSpawn;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (int i = 0; i < m_pool.Size(); ++i)
		{
		}
	}

	void OnCollision(CollisionInfo* collision) override
	{
		CheckpointTriggerComponent* comp = FindComponent(collision->self.GetEntity());
		if (comp)
		{
			m_spawnSystem->SetActiveSpawn(comp->hSpawn);
		}
	}

protected:
	SpawnSystem* m_spawnSystem;
};