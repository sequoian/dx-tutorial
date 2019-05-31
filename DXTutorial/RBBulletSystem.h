#pragma once

#include "ComponentSystem.h"
#include "PrimitiveFactory.h"
#include "Types.h"


struct RBBulletComponent
{
};


class RBBulletSystem : public ComponentSystem<RBBulletComponent>
{
public:
	void AddSystemRefs(EventBus& bus)
	{
		SubscribeToCollisionEvents(bus);
	}

	inline void Execute(float deltaTime) override
	{

	}

private:
	void OnCollision(CollisionInfo* collision) override
	{
		if (!collision->other.IsTrigger())
		{
			m_entityManager->Destroy(collision->self.GetEntity());
		}
	}
};