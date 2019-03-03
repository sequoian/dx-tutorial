#pragma once

#include <unordered_map>
#include "StringId.h"

template <class T>
class ResourcePool
{
public:

	inline T* Create(const char* key)
	{
		StringId hash = GenStringId(key);

		auto result = m_resourceMap.emplace(hash, m_next);
		if (!result.second)
		{
			DEBUG_ERROR("Resource already created: %s", key);
			return nullptr;
		}

		return &result.first->second;
	}


	inline T* Find(const char* key)
	{
		StringId hash = GenStringId(key);
		auto itr = m_resourceMap.find(hash);
		if (itr == m_resourceMap.end())
		{
			return nullptr;
		}
		else
		{
			return &itr->second;
		}
	}


	inline void Destroy(const char* key)
	{
		auto itr = m_resourceMap.find(key);
		if (itr != m_resourceMap.end())
		{
			m_resourceMap.erase(itr);
		}
	}


protected:
	std::unordered_map<StringId, T> m_resourceMap;
	T m_next;
};