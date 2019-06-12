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
	bool StartUp(U32 numComponents, EntityManager& em, EventBus& bus)
	{
		TParent::StartUp(numComponents, em);

		SubscribeToCollisionEvents(bus);

		return true;
	}

	inline void Execute(float deltaTime) override
	{

	}

private:
	void OnCollision(CollisionInfo* collision) override
	{
	}
};