#pragma once

#include <unordered_map>
#include "CompactPool.h"
#include "StringId.h"
#include "Types.h"
#include "Resource.h"


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


	inline U64 Insert(const StringId key, Resource* resource)
	{
		auto itr = m_resourceMap.find(key);
		if (itr == m_resourceMap.end())
		{
			U64 handle = m_pool.InsertObject(resource);
			m_resourceMap.emplace(key, handle);
			
			return handle;
		}
		else
		{
			// resource already exists, return handle
			return itr->second;
		}
	}


	inline Resource* Find(const StringId key)
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


	inline Resource* Get(U64 handle)
	{
		return *m_pool.GetObjectByHandle(handle);
	}


	inline void DestroyByStringId(const StringId key)
	{
		auto itr = m_resourceMap.find(key);
		if (itr != m_resourceMap.end())
		{
			U64 handle = itr->second;
			Resource* resource = *m_pool.GetObjectByHandle(handle);
			resource->ShutDown();
			delete resource;
			m_pool.DestroyObject(handle);
			m_resourceMap.erase(itr);
		}
	}


protected:
	CompactPool<Resource*> m_pool;
	std::unordered_map<StringId, U64> m_resourceMap;
};