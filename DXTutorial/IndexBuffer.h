#include "Graphics.h"


class IndexBuffer
{
public:
	~IndexBuffer();

	bool StartUp(Graphics& graphics, unsigned int numIndices, bool dynamic,
		bool gpuwritable, const unsigned int* data);
	void Select(Graphics& graphics);

private:
	ID3D11Buffer * m_buffer = nullptr;
};
