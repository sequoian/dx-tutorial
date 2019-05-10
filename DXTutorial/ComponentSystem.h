#pragma once

#include <unordered_map>
#include "CompactPool.h"
#include "Types.h"
#include "Entity.h"
#include "EventBus.h"
#include "CollisionInfo.h"

template <class T>
class ComponentSystem
{
public:
	// execute system
	virtual void Execute(float deltaTime) = 0;

	virtual inline bool StartUp(U32 numComponents)
	{
		m_pool.StartUp(numComponents);
		return true;
	}


	virtual inline U64 CreateComponent(Entity e)
	{
		U64 handle = m_pool.CreateObject();

		// set entity map
		m_entityMap.emplace(e.id, handle);

		return handle;
	}


	virtual inline T* FindComponent(Entity e)
	{
		auto itr = m_entityMap.find(e.id);
		if (itr == m_entityMap.end())
		{
			return nullptr;
		}
		else
		{
			return GetComponentByHandle(itr->second);
		}
	}


	virtual inline const T* FindComponentConst(Entity e) const
	{
		auto itr = m_entityMap.find(e.id);
		if (itr == m_entityMap.end())
		{
			return nullptr;
		}
		else
		{
			return GetComponentByHandleConst(itr->second);
		}
	}


	virtual inline T* GetComponentByHandle(U64 handle)
	{
		return m_pool.GetObjectByHandle(handle);
	}


	virtual inline const T* GetComponentByHandleConst(U64 handle) const
	{
		return m_pool.GetObjectByHandleConst(handle);
	}


	virtual inline void DestroyComponent(Entity e)
	{
		auto itr = m_entityMap.find(e.id);
		if (itr != m_entityMap.end())
		{
			m_pool.DestroyObject(itr->second);

			// remove entity from map
			m_entityMap.erase(itr);
		}
	}

protected:

	virtual void SubscribeToCollisionEvents(EventBus& bus)
	{
		bus.Subscribe(this, &ComponentSystem<T>::OnCollisionEvent);
	}

	virtual void OnCollisionEvent(CollisionEvent* collision)
	{
		// test
		Entity a = collision->rigidBodyA.GetEntity();
		Entity b = collision->rigidBodyB.GetEntity();

		// check first rigidbody
		T* component = FindComponent(collision->rigidBodyA.GetEntity());
		if (component)
		{
			CollisionInfo info(collision->rigidBodyA, collision->rigidBodyB, -1, collision->numContactPoints, collision->contactPoints);
			OnCollision(&info);
		}

		// check second rigidbody
		component = FindComponent(collision->rigidBodyB.GetEntity());
		if (component)
		{
			CollisionInfo info(collision->rigidBodyB, collision->rigidBodyA, 1, collision->numContactPoints, collision->contactPoints);
			OnCollision(&info);
		}
	}

	virtual void OnCollision(CollisionInfo* collision)
	{
	}

protected:
	CompactPool<T> m_pool;
	std::unordered_map<U64, U64> m_entityMap;
};