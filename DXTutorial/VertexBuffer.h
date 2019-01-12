#include "Graphics.h"


class VertexBuffer
{
public:
	~VertexBuffer();

	bool StartUp(Graphics& graphics, const VertexFormat& fmt, unsigned int numVerts, bool dynamic, bool gpuwritable, const void* data);
	void Select(Graphics& graphics, unsigned int slot = 0);

	const VertexFormat& GetFormat() const { return m_fmt; }

private:
	VertexFormat m_fmt;
	ID3D11Buffer* m_vb = nullptr;
};
