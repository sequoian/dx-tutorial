#pragma once

#include "Graphics.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexFormat.h"
#include "OBJLoader.h"
#include "Resource.h"
#include <vector>

using namespace std;


class Model : public Resource
{
public:
	~Model();

	bool LoadFromOBJ(Graphics& graphics, const char* filename);
	void Select(Graphics& graphics);
	void Draw(Graphics& graphics);

private:
	VertexBuffer m_vb;
	IndexBuffer m_ib;
	unsigned int m_numIndices = 0;
	bool initialized = false;
};
