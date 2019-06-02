#include "Graphics.h"


class Buffer
{
public:
	static const unsigned int MaxBuffers = 2;

	~Buffer();

	bool CreateConstantBuffer(Graphics& graphics, unsigned int size, bool dynamic, const void* data);

	void* Map(Graphics& graphics);
	void Unmap(Graphics& graphics);

	template<typename T> T* Map(Graphics& graphics)
	{
		return reinterpret_cast<T*>(Map(graphics));
	}

	template<typename T> void MapAndSet(Graphics& graphics, const T& data)
	{
		T* dst = Map<T>(graphics);
		*dst = data;
		Unmap(graphics);
	}

	ID3D11Buffer* GetCurrentBuffer() const { return m_buffers[m_curBuffer]; }

private:
	ID3D11Buffer* m_buffers[MaxBuffers] = {};
	unsigned int m_numBuffers = 0;
	unsigned int m_curBuffer = 0;
};
