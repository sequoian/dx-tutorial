#pragma once

#include <unordered_map>
#include "StringId.h"

template <class T>
class ResourcePool
{
public:

	inline bool Create(StringId key, T* resource)
	{
		auto result = m_resourceMap.emplace(key, m_next);
		resource = &result.first->second;
		if (result.second)
		{
			// resource was created
			return false;
		}
		else
		{
			// resource was already created and simply retrieved
			return true;
		}
	}


	inline T* Find(StringId key)
	{
		auto itr = m_resourceMap.find(key);
		if (itr == m_resourceMap.end())
		{
			DEBUG_WARN("Resource not found");
			return nullptr;
		}
		else
		{
			return &itr->second;
		}
	}


	inline void Destroy(StringId key)
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