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
	float legLength;
	float maxSlopeAngle;
	float angleOfGround;
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

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			LegCastComponent* comp = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->hParentTransform);
			VelocityComponent* velocity = m_velocitySystem->GetComponentByHandle(comp->hParentVelocity);

			auto result = m_physics->RayCast(transform->position, XMVectorAdd(Vector3(0, -comp->legLength, 0), transform->position) );
			if (result.hasHit())
			{
				const auto& ptB = result.m_rayToWorld;
				const auto& ptA = result.m_hitPointWorld;
				const auto& normalOnB = result.m_hitNormalWorld;
				

				// test
				XMVECTOR normal = Physics::VecToDX(normalOnB);
				XMVECTOR penVelocity = XMVectorMultiply(normal, XMVector3Dot(velocity->velocity, normal));
				velocity->velocity -= penVelocity;

				XMVECTOR diff = Physics::VecToDX(ptA - ptB);
				XMVECTOR norm = Physics::VecToDX(normalOnB);
				transform->position += diff;

				// out of date
				//transform->position += XMVectorMultiply(norm, XMVector3Dot(diff, norm));

				

				// print angle of ground
				//XMVECTOR angle = XMVector3AngleBetweenNormals(Physics::VecToDX(normalOnB), Vector3(0, 1, 0));
				//DEBUG_PRINT("%.f", RadiansToDegrees(angle.m128_f32[0]));

				comp->grounded = true;
			}
			else
			{
				comp->grounded = false;
			}
		}
	}

protected:
	TransformSystem * m_transformSystem;
	Physics* m_physics;
	VelocitySystem* m_velocitySystem;
};