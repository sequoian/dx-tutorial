#include "VertexBuffer.h"


VertexBuffer::~VertexBuffer()
{
	if (m_vb != nullptr)
	{
		m_vb->Release();
		m_vb = nullptr;
	}
}


bool VertexBuffer::StartUp(Graphics& graphics, const VertexFormat& fmt, unsigned int numVerts, bool dynamic, bool gpuwritable, const void* data)
{
	m_fmt = fmt;

	m_vb = graphics.CreateVertexBuffer(numVerts, m_fmt.Stride, dynamic, gpuwritable, data);
	if (!m_vb)
	{
		return false;
	}

	return true;
}


void VertexBuffer::Select(Graphics& graphics, unsigned int slot)
{
	graphics.SetVertexBuffer(m_vb, slot, m_fmt.Stride, 0);
}