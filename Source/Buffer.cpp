#include "Buffer.h"


Buffer::~Buffer()
{
	for (int i = 0; i < m_numBuffers; ++i)
	{
		if (m_buffers[i] != nullptr)
		{
			m_buffers[i]->Release();
			m_buffers[i] = nullptr;
		}
	}
}


bool Buffer::CreateConstantBuffer(Graphics& graphics, unsigned int size, bool dynamic, const void* data)
{
	// If the buffer is dynamic, create a second identical buffer for double-buffering
	m_numBuffers = dynamic ? 2 : 1;

	for (int i = 0; i < m_numBuffers; ++i)
	{
		m_buffers[i] = graphics.CreateConstantBuffer(size, dynamic, data);
		if (m_buffers[i] == nullptr)
		{
			return false;
		}
	}

	return true;
}


void* Buffer::Map(Graphics& graphics)
{
	// Swap buffers
	m_curBuffer = ++m_curBuffer % m_numBuffers;

	void* result = graphics.MapBuffer(m_buffers[m_curBuffer]);

	return result;
}


void Buffer::Unmap(Graphics& graphics)
{
	graphics.UnmapBuffer(m_buffers[m_curBuffer]);
}