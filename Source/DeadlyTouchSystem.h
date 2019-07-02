#pragma once

#include "ComponentSystem.h"
#include "Types.h"
#include "AppEvents.h"

struct DeadlyTouchComponent
{
};


class DeadlyTouchSystem : public ComponentSystem<DeadlyTouchComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, EventBus& eventBus)
	{
		Parent::StartUp(numComponents, em);

		SubscribeToCollisionEvents(eventBus);
		m_eventBus = &eventBus;

		return true;
	}

	inline void Execute(float deltaTime) override
	{
	}

protected:
	void OnCollision(CollisionInfo* collision) override
	{
		Entity victim = collision->other.GetEntity();

		if (collision->contactPoints.size() > 0)
		{
			m_eventBus->Publish(&OnDeathEvent(victim));
		}
	}

protected:
	EventBus* m_eventBus;
};