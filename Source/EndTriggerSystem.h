#pragma once

#include "ComponentSystem.h"
#include "DeathSystem.h"
#include "CoinSystem.h"
#include "Timer.h"
#include "WriteLog.h"


struct EndTriggerComponent
{
	Entity player;
};


class EndTriggerSystem : public ComponentSystem<EndTriggerComponent>
{
public:

	bool StartUp(U32 numComponents, EntityManager& em, EventBus& bus, Timer& timer, DeathSystem& death, CoinSystem& coin)
	{
		Parent::StartUp(numComponents, em);

		SubscribeToCollisionEvents(bus);
		m_timer = &timer;
		m_deathSystem = &death;
		m_coinSystem = &coin;

		return true;
	}

	U64 CreateComponent(Entity e, Entity player)
	{
		U64 handle = Parent::CreateComponent(e);
		EndTriggerComponent* comp = GetComponentByHandle(handle);

		comp->player = player;

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
		EndTriggerComponent* comp = FindComponent(collision->self.GetEntity());
		DeathComponent* death = m_deathSystem->FindComponent(comp->player);

		DEBUG_PRINT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
		DEBUG_PRINT("You Win!");
		DEBUG_PRINT("Time taken: %.2f seconds", m_timer->GetTime());
		// subtract 1 from total since this function is called before the final coin is destroyed
		DEBUG_PRINT("Coins collected: %u out of %u", m_coinSystem->GetNumCollected(), m_coinSystem->GetTotal()-1);
		DEBUG_PRINT("Times died: %u", death->deathCount);
		DEBUG_PRINT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	}

private:
	DeathSystem* m_deathSystem;
	CoinSystem* m_coinSystem;
	Timer* m_timer;
};