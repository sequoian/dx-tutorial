#pragma once
#include <vector>
#include <unordered_map>
#include <deque>
#include <stdint.h>
#include <DirectXMath.h>
using namespace DirectX;


typedef uint32_t U32;
typedef uint64_t U64;


struct Entity
{
	U64 id;
};


struct TransformComponent
{
	U64 id;
	XMMATRIX world;
};


template <class T>
class ComponentPool
{
public:
	bool StartUp(U32 poolSize)
	{
		m_pool.reserve(poolSize);
		m_remap.reserve(poolSize);
		m_entityMap.reserve(poolSize);
		return true;
	}


	T& CreateComponent(Entity e)
	{
		U32 idx;
		U32 gen;
		U64 remap;

		if (m_freedIds.size() > m_minFree)
		{
			// reuse an id
			m_next.id = m_freedIds.front();
			m_freedIds.pop_front();

			idx = Idx(m_next.id);
			gen = Gen(m_next.id);

			// generate remap value
			remap = Combine(m_numActive, gen);
			m_remap[idx] = remap;
		}
		else
		{
			idx = m_remap.size();
			gen = 0;

			// create new id and set generation to 0
			m_next.id = Combine(idx, gen);

			// generate remap value
			remap = Combine(m_numActive, gen);
			m_remap.push_back(remap);
		}


		// add component to pool
		m_pool.push_back(m_next);
		m_numActive++;

		// set entity map
		m_entityMap.emplace(e.id, remap);

		// return component
		return m_next;
	}


	T& FindComponent(Entity e)
	{
		std::unordered_map<U64, U64>::iterator itr = m_entityMap.find(e.id);
		if (itr == m_entityMap.end())
		{
			return nullptr;
		}
		else
		{
			return GetComponentById(itr->second);
		}
	}


	T& GetComponentById(U64 id)
	{
		U64 remap = m_remap[Idx(id)];

		if (Gen(id) == Gen(remap))
		{
			return pool[Idx(remap)];
		}
		else
		{
			return nullptr;
		}
	}


	void DestroyComponent(Entity e)
	{
		std::unordered_map<U64, U64>::iterator itr = m_entityMap.find(e.id);
		if (itr == m_entityMap.end())
		{
			return;
		}
		else
		{
			DestroyComponentById(itr->second);

			// remove entity from map
			m_entityMap.erase(itr);
		}
	}


	void DestroyComponentById(U64 id)
	{
		U32 idxToRemap = Idx(id);
		U32 gen = Gen(id);
		U64 remapId = m_remap[idxToRemap];
		U32 idxToPool = Idx(remapId);
		U32 remapGen = Gen(remapId);

		if (gen == remapGen)
		{
			// swap back component with obsolete component
			T back = m_pool.back();
			m_pool[idxToPool] = back;

			// reduce pool
			m_pool.pop_back();
			m_numActive--;

			// update remap of swapped component to new pool idx
			m_remap[Idx(back.id)] = Combine(idxToPool, Gen(back.id));

			// increment gen on remap of destroyed component to invalidate stale ids
			gen++;
			m_remap[idxToRemap] = Combine(0, gen);

			// add id with incremented gen to freed ids
			m_freedIds.push_back(Combine(idxToRemap, gen));
		}
	}


private:
	U32 Gen(U64 id) const
	{
		return id & m_bitMask;
	}


	U32 Idx(U64 id) const
	{
		return (id >> m_bitShift) & m_bitMask;
	}


	U64 Combine(U32 idx, U32 gen)
	{
		return ((U64)gen << m_bitShift) | idx;
	}


private:
	std::vector<T> m_pool;
	std::vector<U64> m_remap;
	std::deque<U32> m_freedIds;
	std::unordered_map<U64, U64> m_entityMap;

	T m_next;
	U32 m_numActive = 0;

	static const U32 m_bitShift = 32;
	static const U32 m_bitMask = (1 << m_bitShift) - 1;
	static const U32 m_minFree = 2048;
};