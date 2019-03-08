#pragma once
#include <vector>
#include <deque>
#include "Types.h"


template <class T>
class CompactPool
{
public:
	typedef typename std::vector<T>::iterator iterator;
	typedef typename std::vector<T>::const_iterator const_iterator;

public:
	inline bool StartUp(U32 poolSize)
	{
		m_pool.reserve(poolSize);
		m_remapToPool.reserve(poolSize);
		m_remapToHandle.reserve(poolSize);
		return true;
	}


	inline U64 CreateObject()
	{
		U64 handle;
		U32 idx;
		U32 gen;
		U64 remapHandle;

		if (m_freedHandles.size() > m_minFree)
		{
			// reuse a handle
			handle = m_freedHandles.front();
			m_freedHandles.pop_front();

			idx = Idx(handle);
			gen = Gen(handle);

			// generate remap value
			remapHandle = Combine(m_numActive, gen);
			m_remapToPool[idx] = remapHandle;
		}
		else
		{
			// create new handle and set generation to 0
			idx = m_remapToPool.size();
			gen = 0;
			handle = Combine(idx, gen);

			// generate remap value
			remapHandle = Combine(m_numActive, gen);
			m_remapToPool.push_back(remapHandle);
		}

		// add component to pool
		m_pool.push_back(m_next);
		m_remapToHandle.push_back(handle);
		m_numActive++;

		// return component handle
		return handle;
	}


	inline T* GetObjectByHandle(U64 handle)
	{
		U64 remap = m_remapToPool[Idx(handle)];

		if (Gen(handle) == Gen(remap))
		{
			return &m_pool[Idx(remap)];
		}
		else
		{
			return nullptr;
		}
	}

	inline const T* GetObjectByHandleConst(U64 handle) const
	{
		U64 remap = m_remapToPool[Idx(handle)];

		if (Gen(handle) == Gen(remap))
		{
			return &m_pool[Idx(remap)];
		}
		else
		{
			return nullptr;
		}
	}


	inline void DestroyObject(U64 handle)
	{
		U32 idxToRemap = Idx(handle);
		U32 gen = Gen(handle);
		U64 remapHandle = m_remapToPool[idxToRemap];
		U32 idxToPool = Idx(remapHandle);
		U32 remapGen = Gen(remapHandle);

		if (gen == remapGen)
		{
			// swap back component with obsolete component and reduce pool
			T back = m_pool.back();
			m_pool[idxToPool] = back;
			m_pool.pop_back();
			m_numActive--;

			// update pool idx to handle remap
			U64 swappedHandle = m_remapToHandle.back();
			m_remapToHandle[idxToPool] = swappedHandle;
			m_remapToHandle.pop_back();

			// update handle to pool idx remap
			m_remapToPool[Idx(swappedHandle)] = Combine(idxToPool, Gen(swappedHandle));

			// increment gen on remap of destroyed component to invalidate stale handles
			gen++;
			m_remapToPool[idxToRemap] = Combine(0, gen);

			// add handle with incremented gen to freed handles
			m_freedHandles.push_back(Combine(idxToRemap, gen));
		}
	}


	inline U32 Size() const
	{
		return m_numActive;
	}


	inline T* operator[] (I32 idx)
	{
		return &m_pool[idx];
	}


	iterator Begin()
	{
		return m_pool.begin();
	}


	iterator End()
	{
		return m_pool.end();
	}


	const_iterator CBegin()
	{
		return m_pool.cbegin();
	}


	const_iterator CEnd()
	{
		return m_pool.cend();
	}


private:
	inline U32 Idx(U64 handle) const
	{
		return handle & m_bitMask;
	}


	inline U32 Gen(U64 handle) const
	{
		return (handle >> m_bitShift) & m_bitMask;
	}


	inline U64 Combine(U32 idx, U32 gen) const
	{
		return ((U64)gen << m_bitShift) | idx;
	}

private:
	std::vector<T> m_pool;
	std::vector<U64> m_remapToPool;
	std::vector<U64> m_remapToHandle;
	std::deque<U64> m_freedHandles;

	T m_next;
	U32 m_numActive = 0;

	static const U32 m_bitShift = 32;
	static const U32 m_bitMask = ((U64)1 << m_bitShift) - 1;
	static const U32 m_minFree = 2048;
};
