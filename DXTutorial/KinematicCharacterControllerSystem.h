#pragma once

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
		if (!collision->other.IsDynamic() && !collision->other.IsTrigger())
		{
			BlockSelf(collision);
		}
	}

	void BlockSelf(CollisionInfo* collision)
	{
		TransformComponent* transform = m_transformSystem->FindComponent(collision->self.GetEntity());

		for (auto& point : collision->contactPoints)
		{
			const auto& ptA = point.getPositionWorldOnA();
			const auto& ptB = point.getPositionWorldOnB();
			const auto& normalOnB = point.m_normalWorldOnB;

			XMVECTOR diff = Physics::VecToDX(ptA - ptB) * collision->direction;
			XMVECTOR norm = Physics::VecToDX(normalOnB);
			transform->position += XMVectorMultiply(norm, XMVector3Dot(diff, norm));
		}
	}

private:
	TransformSystem* m_transformSystem;
	VelocitySystem* m_velocitySystem;
};