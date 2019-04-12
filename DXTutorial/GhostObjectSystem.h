#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "Physics.h"
#include "Types.h"


struct GhostObjectComponent
{
	U64 transform = 0;
	btPairCachingGhostObject* ghostObject = nullptr;
};


class GhostObjectSystem : public ComponentSystem<GhostObjectComponent>
{
public:
	void AddSystemRefs(TransformSystem* transformSystem, Physics* physics)
	{
		m_transformSystem = transformSystem;
		m_physics = physics;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			GhostObjectComponent* ghost = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(ghost->transform);

			btQuaternion rotation = Physics::QuatFromDX(transform->rotation);
			btVector3 position = Physics::VecFromDX(transform->position);
			btTransform t(rotation, position);
			ghost->ghostObject->setWorldTransform(t);

			//DEBUG_PRINT("%f, %f, %f", t.getOrigin().getX(), t.getOrigin().getY(), t.getOrigin().getZ());

			GetCollisions(ghost->ghostObject);
		}
	}

private:
	void GetCollisions(btPairCachingGhostObject* ghostObject)
	{
		btManifoldArray manifoldArray;
		btBroadphasePairArray& pairArray =
			ghostObject->getOverlappingPairCache()->getOverlappingPairArray();

		int numPairs = pairArray.size();

		for (int i = 0; i < numPairs; ++i)
		{
			manifoldArray.clear();

			const btBroadphasePair& pair = pairArray[i];

			btBroadphasePair* collisionPair =
				m_physics->GetWorld()->getPairCache()->findPair(
					pair.m_pProxy0, pair.m_pProxy1);

			if (!collisionPair) continue;

			if (collisionPair->m_algorithm)
				collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

			if (manifoldArray.size() > 0)
			{
				DEBUG_PRINT("collided!");
			}

			for (int j = 0; j < manifoldArray.size(); j++)
			{
				btPersistentManifold* manifold = manifoldArray[j];

				bool isFirstBody = manifold->getBody0() == ghostObject;

				btScalar direction = isFirstBody ? btScalar(-1.0) : btScalar(1.0);

				for (int p = 0; p < manifold->getNumContacts(); ++p)
				{
					const btManifoldPoint& pt = manifold->getContactPoint(p);

					if (pt.getDistance() < 0.f)
					{
						const btVector3& ptA = pt.getPositionWorldOnA();
						const btVector3& ptB = pt.getPositionWorldOnB();
						const btVector3& normalOnB = pt.m_normalWorldOnB;

						// handle collisions here
					}
				}
			}
		}
	}

private:
	TransformSystem* m_transformSystem;
	Physics* m_physics;
};