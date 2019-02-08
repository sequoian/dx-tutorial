#pragma once

#include <vector>
#include <deque>
#include "Entity.h"
#include "Assert.h"

const unsigned int minFreeIndices = 2048;

class EntityManager
{
public:
	// Pass the number of entities expected to be created
	bool StartUp(unsigned int numEntities)
	{
		m_usedGenerations.reserve(numEntities);
	}

	Entity CreateEntity()
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

		// Return entity, setting index and generation
		m_next.id = ((uint64_t)m_usedGenerations[idx] << entityGenerationBits) | idx;
		return m_next;
	}

	bool IsAlive(Entity e)
	{
		// Enitty is dead if its generation does not match with the stored genration at its index
		return m_usedGenerations[e.index()] == e.generation();
	}

	void Destroy(Entity e)
	{
		const unsigned int idx = e.index();

		// Increment the stored generation to invalidate alive checks
		++m_usedGenerations[idx];

		// store index for possible resuse
		m_freedIndices.push_back(idx);
	}

private:
	Entity m_next;
	std::vector<unsigned char> m_usedGenerations;
	std::deque<unsigned int> m_freedIndices;

};