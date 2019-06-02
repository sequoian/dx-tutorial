#pragma once

#include <vector>
#include <deque>
#include <unordered_map>
#include "Entity.h"
#include "Assert.h"
#include "ComponentSystem.h"
#include "WriteLog.h"

class ComponentSystemBase;

class OwnedComponent
{
public:
	OwnedComponent() {}

	OwnedComponent(ComponentSystemBase* i, U64 h)
	{
		componentSystem = i;
		componentHandle = h;
	}
	ComponentSystemBase* componentSystem;
	U64 componentHandle;
};

const unsigned int minFreeIndices = 2048;

class EntityManager
{
public:
	// Pass the number of entities expected to be created
	bool StartUp(unsigned int numEntities);

	Entity CreateEntity(U32 numComponents = 0);

	bool IsAlive(Entity e);

	void Destroy(Entity e);

	void AddComponentToEntity(Entity e, ComponentSystemBase* system, U64 handle);

	void EndFrame();

protected:
	void DestroyEntity(Entity e);
	void DestroyComponentsOfEntity(Entity e);

private:
	typedef std::vector<OwnedComponent> ComponentList;
	Entity m_next;
	std::vector<unsigned char> m_usedGenerations;
	std::deque<unsigned int> m_freedIndices;
	std::unordered_map<U64, ComponentList> m_componentMap;
	std::vector<Entity> m_condemnedEntities;
};