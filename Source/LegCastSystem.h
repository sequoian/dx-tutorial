#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "Physics.h"
#include "MathUtility.h"

struct LegCastComponent
{
	U64 hParentTransform;
	float legLength;
};

class LegCastSystem : public ComponentSystem<LegCastComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem, Physics& physics)
	{
		Parent::StartUp(numComponents, em);
		m_transformSystem = &transformSystem;
		m_physics = &physics;
		return true;
	}

	U64 CreateComponent(Entity e, U64 hParentTransform, float legLength)
	{
		U64 handle = Parent::CreateComponent(e);
		LegCastComponent* comp = GetComponentByHandle(handle);

		comp->hParentTransform = hParentTransform;
		comp->legLength = legLength;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			LegCastComponent* comp = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->hParentTransform);

			auto result = m_physics->RayCast(transform->position, XMVectorAdd(Vector3(0, -comp->legLength, 0), transform->position) );
			if (result.hasHit())
			{
				const auto& ptB = result.m_rayToWorld;
				const auto& ptA = result.m_hitPointWorld;
				const auto& normalOnB = result.m_hitNormalWorld;

				XMVECTOR diff = Physics::VecToDX(ptA - ptB);
				XMVECTOR norm = Physics::VecToDX(normalOnB);
				transform->position += XMVectorMultiply(norm, XMVector3Dot(diff, norm));
			}
		}
	}

protected:
	TransformSystem * m_transformSystem;
	Physics* m_physics;
};