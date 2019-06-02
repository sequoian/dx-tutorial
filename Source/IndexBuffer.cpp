#include "IndexBuffer.h"


IndexBuffer::~IndexBuffer()
{
	if (m_buffer != nullptr)
	{
		m_buffer->Release();
		m_buffer = nullptr;
	}
}


bool IndexBuffer::StartUp(Graphics& graphics, unsigned int numIndices, bool dynamic,
	bool gpuwritable, const unsigned int* data)
{
	m_buffer = graphics.CreateIndexBuffer(numIndices, dynamic, gpuwritable, data);
	if (!m_buffer)
	{
		return false;
	}
	
	return true;
}


void IndexBuffer::Select(Graphics& graphics)
{
	graphics.SetIndexBuffer(m_buffer);
}