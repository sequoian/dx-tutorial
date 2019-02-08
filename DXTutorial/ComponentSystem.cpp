#include "ComponentSystem.h"


template <class T>
bool ComponentSystem<T>::StartUp(U32 numComponents)
{
	m_pool.StartUp(numComponents);
}


template <class T>
U64 ComponentSystem<T>::CreateComponent(Entity e)
{
	U64 handle = m_pool.CreateObject();

	// set entity map
	m_entityMap.emplace(e.id, handle);

	return handle;
}


template <class T>
T& ComponentSystem<T>::FindComponent(Entity e)
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


template <class T>
T& ComponentSystem<T>::GetComponentByHandle(U64 handle)
{
	T component;
	if (m_pool.GetObject(handle, &component))
	{
		return component;
	}
	else
	{
		return nullptr;
	}
}


template <class T>
void ComponentSystem<T>::DestroyComponent(Entity e) {
	auto itr = m_entityMap.find(e.id);
	if (itr != m_entityMap.end())
	{
		DestroyComponentByHandle(itr->second);

		// remove entity from map
		m_entityMap.erase(itr);
	}
}


template <class T>
void ComponentSystem<T>::DestroyComponentByHandle(U64 handle)
{
	m_pool.DestroyObject(handle);
}