#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "Physics.h"
#include "Types.h"


struct ColliderComponent
{
	btCollisionShape* shape = nullptr;
};


class ColliderSystem : public ComponentSystem<ColliderComponent>
{
public:

	inline void Execute(float deltaTime) override
	{
	}
};