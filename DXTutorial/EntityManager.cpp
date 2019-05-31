#include "EntityManager.h"


bool EntityManager::StartUp(unsigned int numEntities)
{
	m_usedGenerations.reserve(numEntities);
	return true;
}


Entity EntityManager::CreateEntity(U32 numComponents)
{
	uint32_t idx;

	if (m_freedIndices.size() > minFreeIndices)
	{
		// If there are too many freed indices, begin reusing them
		idx = m_freedIndices.front();
		m_freedIndices.pop_front();
	}
	else
	{
		// Create a fresh index with the generation starting at 0
		m_usedGenerations.push_back(0);
		idx = m_usedGenerations.size() - 1;
		ASSERT(idx < (uint64_t)1 << entityIndexBits);
	}

	// set index and generation
	m_next.id = ((uint64_t)m_usedGenerations[idx] << entityGenerationBits) | idx;

	// add entity to component map and reserve space in vector
	auto itr = m_componentMap[m_next.id];
	itr.reserve(numComponents);

	return m_next;
}


bool EntityManager::IsAlive(Entity e)
{
	// Entity is dead if its generation does not match with the stored genration at its index
	return m_usedGenerations[e.index()] == e.generation();
}


void EntityManager::Destroy(Entity e)
{
	m_condemnedEntities.push_back(e);
}


void EntityManager::AddComponentToEntity(Entity e, ComponentSystemBase* system, U64 handle)
{
	auto itr = m_componentMap.find(e.id);
	if (itr == m_componentMap.end())
	{
		DEBUG_ERROR("Cannot add a component to an entity that does not exist");
		return;
	}

	itr->second.emplace_back(OwnedComponent(system, handle));
}


void EntityManager::EndFrame()
{
	// destroy condemned entities at the end of the frame
	for (int i = 0; i < m_condemnedEntities.size(); ++i)
	{
		DestroyEntity(m_condemnedEntities[i]);
	}
	m_condemnedEntities.clear();
}


void EntityManager::DestroyEntity(Entity e)
{
	DestroyComponentsOfEntity(e);

	const unsigned int idx = e.index();

	// Increment the stored generation to invalidate alive checks
	++m_usedGenerations[idx];

	// store index for possible resuse
	m_freedIndices.push_back(idx);
}


void EntityManager::DestroyComponentsOfEntity(Entity e)
{
	auto entityItr = m_componentMap.find(e.id);
	if (entityItr == m_componentMap.end())
	{
		return;
	}

	auto comps = entityItr->second;

	for (int i = 0; i < comps.size(); ++i)
	{
		comps[i].componentSystem->DestroyComponent(e);
	}
}