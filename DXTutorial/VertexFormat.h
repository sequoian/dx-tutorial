#include <d3d11.h>
#include "WriteLog.h"


// Helper class to provide a description the binary layout and purpose of a
// vertex buffer
class VertexFormat
{
public:
	static const unsigned int MaxInputs = 8;

	unsigned int NumInputs = 0;
	unsigned int Stride = 0;
	D3D11_INPUT_ELEMENT_DESC Inputs[MaxInputs] = {};

	void Reset() { NumInputs = 0; Stride = 0; }
	void AddPadding(unsigned int size) { Stride += size; }
	bool AddElement(DXGI_FORMAT fmt, unsigned int size, const char* semanticName,
		unsigned int semanticIndex = 0,
		unsigned int inputSlot = 0,
		D3D11_INPUT_CLASSIFICATION inputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
		unsigned int stepRate = 0);
};


// Vertex data that contains a position and a color
class VertPosColor
{
public:
	float x, y, z;
	unsigned int color;

	static VertexFormat GetVertexFormat();
};


// Vertex data that contains a position and a texture coordinate
class VertPosUV
{
public:
	float x, y, z;
	float u, v;

	static VertexFormat GetVertexFormat();
};


// Helper function to create a color uint from red, green, blue, and alpha values
inline unsigned int MakeColorUInt(unsigned char r, unsigned char g, unsigned char b,
	unsigned char a)
{
	return (r | (g << 8) | (b << 16) | (a << 24));
}
