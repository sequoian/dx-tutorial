#pragma once
#include <vector>
#include <deque>
#include "Types.h"


template <class T>
class CompactPool
{
public:
	bool StartUp(U32 poolSize);
	U64 CreateObject();
	bool GetObject(U64 handle, T& object);
	void DestroyObject(U64 handle);
	U32 Size() const;
	T& operator[] (I32);

private:
	U32 Gen(U64 handle) const
	{
		return handle & m_bitMask;
	}

	U32 Idx(U64 handle) const
	{
		return (handle >> m_bitShift) & m_bitMask;
	}

	U64 Combine(U32 idx, U32 gen) const
	{
		return ((U64)gen << m_bitShift) | idx;
	}

private:
	std::vector<T> m_pool;
	std::vector<U64> m_remapToPool;
	std::vector<U64> m_remapToHandle;
	std::deque<U32> m_freedHandles;

	T m_next;
	U32 m_numActive = 0;

	static const U32 m_bitShift = 32;
	static const U32 m_bitMask = (1 << m_bitShift) - 1;
	static const U32 m_minFree = 2048;
};
