// these must be defined once here
#define TINYOBJLOADER_IMPLEMENTATION
#include "../Headers/tiny_obj_loader.h"

#include "OBJLoader.h"
#include <unordered_map>

using namespace tinyobj;
using namespace std;

namespace std {
	bool operator==(const index_t& a, const index_t& b)
	{
		return a.vertex_index == b.vertex_index &&
			a.normal_index == b.normal_index &&
			a.texcoord_index == b.texcoord_index;
	}
	template<> struct hash<index_t>
	{
		size_t operator() (const index_t& idx) const
		{
			hash<int> inthash;
			return inthash(idx.vertex_index) ^
				inthash(idx.normal_index) ^
				inthash(idx.texcoord_index);
		}
	};
}


bool OBJLoader::LoadFromFile(const char* filename, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
	attrib_t attrib;
	vector<shape_t> shapes;
	vector<material_t> materials;
	string err;

	bool ret = LoadObj(&attrib, &shapes, &materials, NULL, &err, filename);
	if (!err.empty())
	{
		// `err` may contain warning message. May want to output it here
		DEBUG_ERROR(err.c_str());
	}

	if (!ret)
		return false;

	unordered_map<index_t, unsigned int> idxmap;

	// Loop over shapes
	for (const shape_t& shape : shapes)
	{
		// Loop over faces(polygon)
		size_t idxOffset = 0;
		for (size_t fv : shape.mesh.num_face_vertices)
		{
			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++)
			{
				const index_t& idx = shape.mesh.indices[idxOffset + v];
				idxmap.try_emplace(idx, (unsigned int)idxmap.size());
			}
			idxOffset += fv;
		}
	}

	// Pre-fill our vertex list to size
	size_t numVerts = idxmap.size();
	vertices.resize(numVerts);

	// Iterate the unordered_map to fill the vertex data
	for (const pair<index_t, unsigned int>& entry : idxmap)
	{
		Vertex& vert = vertices[entry.second];
		vert.Position[0] = attrib.vertices[3 * entry.first.vertex_index + 0];
		vert.Position[1] = attrib.vertices[3 * entry.first.vertex_index + 1];
		vert.Position[2] = attrib.vertices[3 * entry.first.vertex_index + 2];
		vert.Normal[0] = attrib.normals[3 * entry.first.normal_index + 0];
		vert.Normal[1] = attrib.normals[3 * entry.first.normal_index + 1];
		vert.Normal[2] = attrib.normals[3 * entry.first.normal_index + 2];
		vert.UV[0] = attrib.texcoords[2 * entry.first.texcoord_index + 0];
		vert.UV[1] = attrib.texcoords[2 * entry.first.texcoord_index + 1];
		vert.Color[0] = attrib.colors[3 * entry.first.vertex_index + 0];
		vert.Color[1] = attrib.colors[3 * entry.first.vertex_index + 1];
		vert.Color[2] = attrib.colors[3 * entry.first.vertex_index + 2];
	}

	// Loop over shapes
	for (const shape_t& shape : shapes)
	{
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (unsigned int fv : shape.mesh.num_face_vertices)
		{
			for (unsigned int i = 2; i < fv; ++i) {
				index_t idx0 = shape.mesh.indices[index_offset + 0];
				index_t idx1 = shape.mesh.indices[index_offset + i - 1];
				index_t idx2 = shape.mesh.indices[index_offset + i];

				indices.push_back(idxmap[idx0]);
				indices.push_back(idxmap[idx1]);
				indices.push_back(idxmap[idx2]);
			}
			index_offset += fv;
		}
	}

	return true;
}

