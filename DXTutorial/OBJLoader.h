#pragma once

#include <vector>
#include "WriteLog.h"

class OBJLoader
{
public:

	struct Vertex
	{
		float Position[3] = {};
		float Normal[3] = {};
		float UV[2] = {};
		float Color[4] = { 1.0f, 1.0, 1.0f, 1.0f };
	};

	static bool LoadFromFile(const char* filename, std::vector<Vertex>& vertices,
		std::vector<unsigned int>& indices);
};