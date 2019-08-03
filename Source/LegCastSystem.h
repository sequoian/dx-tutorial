#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "Physics.h"
#include <math.h>
#include "MathUtility.h"
#include "VelocitySystem.h"

struct LegCastComponent
{
	U64 hParentTransform;
	U64 hParentVelocity;
	XMVECTOR groundNormal;
	float legLength;
	float maxSlopeAngle;
	bool grounded;
};

class LegCastSystem : public ComponentSystem<LegCastComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem, Physics& physics,
				 VelocitySystem& vs)
	{
		Parent::StartUp(numComponents, em);
		m_transformSystem = &transformSystem;
		m_physics = &physics;
		m_velocitySystem = &vs;
		return true;
	}

	U64 CreateComponent(Entity e, U64 hParentTransform, U64 hVelocity, float legLength, float maxSlopeAngle = 45)
	{
		U64 handle = Parent::CreateComponent(e);
		LegCastComponent* comp = GetComponentByHandle(handle);

		comp->hParentTransform = hParentTransform;
		comp->legLength = legLength;
		comp->maxSlopeAngle = maxSlopeAngle;
		comp->hParentVelocity = hVelocity;
		comp->grounded = false;
		comp->groundNormal = Vector3(0, 1, 0);

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			LegCastComponent* comp = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->hParentTransform);
			VelocityComponent* velocity = m_velocitySystem->GetComponentByHandle(comp->hParentVelocity);

			XMVECTOR rayStart = transform->position;
			XMVECTOR rayEnd = XMVectorAdd(Vector3(0, -comp->legLength, 0), rayStart);

			// do not vault onto ledges while jumping
			if (!comp->grounded && velocity->velocity.m128_f32[1] > 0)
			{
				return;
			}

			// This would be more efficient to use the closest raycast function combined with collision masks to filter out triggers
			auto result = m_physics->RayCastAll(rayStart, rayEnd);
			if (result.hasHit())
			{
				RigidBody rigidBody;
				int idx = -1;
				float closestFraction = 1;
				for (int i = 0; i < result.m_collisionObjects.size(); ++i)
				{
					if (result.m_hitFractions[i] < closestFraction)
					{
						const btRigidBody* crb = static_cast<const btRigidBody*>(result.m_collisionObjects.at(i));
						btRigidBody* rb = const_cast<btRigidBody*>(crb);
						rigidBody = RigidBody(rb);
						if (!rigidBody.IsTrigger())
						{
							idx = i;
							closestFraction = result.m_hitFractions[i];
						}
					}
				}

				// return if it's only hit triggers
				if (idx < 0)
				{
					comp->grounded = false;
					return;
				}

				const auto& ptB = result.m_rayToWorld + btVector3(0, 0.01, 0);
				const auto& ptA = result.m_hitPointWorld[idx];
				const auto& normalOnB = result.m_hitNormalWorld[idx];

				// reposition above ground
				XMVECTOR normal = Physics::VecToDX(normalOnB);
				comp->groundNormal = normal;
				XMVECTOR diff = Physics::VecToDX(ptA - ptB);
				transform->position += XMVectorMultiply(normal, XMVector3Dot(diff, normal));

				// adjust velocity
				XMVECTOR dot = XMVector3Dot(normal, velocity->velocity);
				velocity->velocity -= XMVectorScale(normal, dot.m128_f32[0]);
				
				comp->grounded = true;
			}
			else
			{
				comp->grounded = false;
				comp->groundNormal = Vector3(0, 1, 0);
			}
		}
	}

protected:
	TransformSystem * m_transformSystem;
	Physics* m_physics;
	VelocitySystem* m_velocitySystem;
};