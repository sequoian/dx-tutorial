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
	virtual void Execute() = 0;

	virtual bool StartUp(U32);
	virtual U64 CreateComponent(Entity e);
	virtual T& FindComponent(Entity e);
	virtual inline T& GetComponentByHandle(U64 handle);
	virtual void DestroyComponent(Entity e);
	virtual void DestroyComponentByHandle(U64 handle);

private:
	CompactPool<T> m_pool;
	std::unordered_map<U64, U64> m_entityMap;
};