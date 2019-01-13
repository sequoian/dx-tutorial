#include "Model.h"


Model::~Model()
{

}


bool Model::LoadFromOBJ(Graphics& graphics, const char* filename)
{
	vector<OBJLoader::Vertex> objVerts;
	vector<unsigned int> objIndices;

	if (!OBJLoader::LoadFromFile(filename, objVerts, objIndices))
	{
		return false;
	}

	vector<VertPosNormUVColor> vertData;
	vertData.resize(objVerts.size());
	for (size_t i = 0; i < objVerts.size(); ++i)
	{
		vertData[i].x = objVerts[i].Position[0];
		vertData[i].y = objVerts[i].Position[1];
		vertData[i].z = objVerts[i].Position[2];
		vertData[i].nx = objVerts[i].Normal[0];
		vertData[i].ny = objVerts[i].Normal[1];
		vertData[i].nz = objVerts[i].Normal[2];
		vertData[i].u = objVerts[i].UV[0];
		vertData[i].v = objVerts[i].UV[1];
		vertData[i].color = MakeColorUInt(objVerts[i].Color[0],
			objVerts[i].Color[1],
			objVerts[i].Color[2],
			objVerts[i].Color[3]);
	}

	// init vert buffer
	if (!m_vb.StartUp(graphics, VertPosNormUVColor::GetVertexFormat(), vertData.size(), false, false, &vertData[0]))
	{
		return false;
	}

	// init index buffer
	m_numIndices = objIndices.size();
	if (!m_ib.StartUp(graphics, m_numIndices, false, false, &objIndices[0]))
	{
		return false;
	}

	return true;
}


void Model::Select(Graphics& graphics)
{
	m_vb.Select(graphics);
	m_ib.Select(graphics);
}


void Model::Draw(Graphics& graphics)
{
	graphics.DrawIndexed(m_numIndices);
}