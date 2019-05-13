#include "TransformSystem.h"
#include "Physics.h"
#include "Types.h"
#include "RigidBody.h"
#include "EventBus.h"
#include "VelocitySystem.h"


struct KinematicCharacterControllerComponent
{
	U64 transform = 0;
};


class KinematicCharacterControllerSystem : public ComponentSystem<KinematicCharacterControllerComponent>
{
public:
	void AddSystemRefs(TransformSystem* transformSystem, EventBus& eventBus)
	{
		m_transformSystem = transformSystem;
		SubscribeToCollisionEvents(eventBus);
	}

	inline void Execute(float deltaTime) override
	{
	}

private:
	void OnCollision(CollisionInfo* collision) override
	{
		if (collision->other.IsTrigger())
		{
			DEBUG_PRINT("trigger hit");
		}
		else if (collision->other.IsDynamic())
		{
			DEBUG_PRINT("dynamic hit");
		}
		else
		{
			DEBUG_PRINT("do a block");
		}
	}

private:
	TransformSystem* m_transformSystem;
	VelocitySystem* m_velocitySystem;
};