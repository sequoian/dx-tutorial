#pragma once

#include <unordered_map>
#include "CompactPool.h"
#include "StringId.h"
#include "Types.h"

template <class T>
class ResourcePool
{
public:

	inline bool StartUp(const U32 poolSize)
	{
		if (m_pool.StartUp(poolSize))
		{
			return true;
		}
	}

	// returns true if the resource was created or false if the resource already exists
	inline bool Create(const StringId key, U64& handle)
	{
		auto itr = m_resourceMap.find(key);
		if (itr != m_resourceMap.end())
		{
			// resource was already created so return it
			handle = itr->second;
			return false;
		}
		else
		{
			// create resource
			handle = m_pool.CreateObject();
			m_resourceMap.emplace(key, handle);
			return true;
		}
	}


	inline T* Find(const StringId key)
	{
		auto itr = m_resourceMap.find(key);
		if (itr == m_resourceMap.end())
		{
			return nullptr;
		}
		else
		{
			return Get(itr->second);
		}
	}


	inline T* Get(U64 handle)
	{
		return m_pool.GetObject(handle);
	}


	inline void Destroy(const StringId key)
	{
		auto itr = m_resourceMap.find(key);
		if (itr != m_resourceMap.end())
		{
			DestroyByHandle(itr->second);
			m_resourceMap.erase(itr);
		}
	}

	inline void DestroyByHandle(U64 handle)
	{
		m_pool.DestroyObject(handle);
	}


protected:
	CompactPool<T> m_pool;
	std::unordered_map<StringId, U64> m_resourceMap;
	T m_next;
};