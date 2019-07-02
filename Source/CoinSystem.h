#pragma once

#include "TransformSystem.h"
#include "Physics.h"
#include "Types.h"
#include "RigidBody.h"
#include "EventBus.h"


struct CoinComponent
{
	bool collected;
};


class CoinSystem : public ComponentSystem<CoinComponent>
{
public:

	bool StartUp(U32 numComponents, EntityManager& em, EventBus& eventBus)
	{
		Parent::StartUp(numComponents, em);

		SubscribeToCollisionEvents(eventBus);

		return true;
	}

	U64 CreateComponent(Entity e)
	{
		U64 handle = Parent::CreateComponent(e);
		CoinComponent* comp = GetComponentByHandle(handle);

		comp->collected = false;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
	}

private:
	void OnCollision(CollisionInfo* collision) override
	{
		CoinComponent* coin = FindComponent(collision->self.GetEntity());
		if (!coin->collected)
		{
			coin->collected = true;
			coinsCollected++;
			m_entityManager->Destroy(collision->self.GetEntity());
			DEBUG_PRINT("Collected %u coins", coinsCollected);
		}
	}

private:
	U32 coinsCollected = 0;
};