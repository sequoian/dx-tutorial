#pragma once

#include <unordered_map>
#include "CompactPool.h"
#include "Types.h"
#include "Entity.h"

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
			DestroyComponentByHandle(itr->second);

			// remove entity from map
			m_entityMap.erase(itr);
		}
	}


	virtual inline void DestroyComponentByHandle(U64 handle)
	{
		m_pool.DestroyObject(handle);
	}


protected:
	CompactPool<T> m_pool;
	std::unordered_map<U64, U64> m_entityMap;
};