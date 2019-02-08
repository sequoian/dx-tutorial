#include "CompactPool.h"


template <class T>
bool CompactPool<T>::StartUp(U32 poolSize)
{
	m_pool.reserve(poolSize);
	m_remapToPool.reserve(poolSize);
	m_remapToHandle.reserve(poolSize);
	return true;
}


template <class T>
U64 CompactPool<T>::CreateObject()
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


template <class T>
bool CompactPool<T>::GetObject(U64 handle, T& object)
{
	U64 remap = m_remapToPool[Idx(handle)];

	if (Gen(handle) == Gen(remap))
	{
		object = pool[Idx(remap)];
		return true;
	}
	else
	{
		return false;
	}
}


template <class T>
void CompactPool<T>::DestroyObject(U64 handle)
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
		m_remapToHandle[idxToPool] = back;
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


template <class T>
U32 CompactPool<T>::Size() const
{
	return m_numActive;
}


template <class T>
T& CompactPool<T>::operator[] (I32 idx)
{
	return m_pool[idx];
}