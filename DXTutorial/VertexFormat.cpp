#include "VertexFormat.h"


bool VertexFormat::AddElement(DXGI_FORMAT fmt, unsigned int size,
	const char* semanticName, unsigned int semanticIndex,
	unsigned int inputSlot,
	D3D11_INPUT_CLASSIFICATION inputSlotClass,
	unsigned int stepRate)
{
	unsigned int index = NumInputs;
	if (index >= MaxInputs)
	{
		WriteLog("Vertex format exceeds max inputs\n");
		return false;
	}

	Inputs[index].SemanticName = semanticName;
	Inputs[index].SemanticIndex = semanticIndex;
	Inputs[index].InputSlot = inputSlot;
	Inputs[index].Format = fmt;
	Inputs[index].AlignedByteOffset = Stride;
	Inputs[index].InputSlotClass = inputSlotClass;
	Inputs[index].InstanceDataStepRate = stepRate;

	++NumInputs;
	Stride += size;
	return true;
}

VertexFormat VertPosColor::GetVertexFormat() 
{
	VertexFormat fmt;
	fmt.AddElement(DXGI_FORMAT_R32G32B32_FLOAT, sizeof(float) * 3, "POSITION");
	fmt.AddElement(DXGI_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned char) * 4, "TEXCOORD");
	return fmt;
}


VertexFormat VertPosUV::GetVertexFormat()
{
	VertexFormat fmt;
	fmt.AddElement(DXGI_FORMAT_R32G32B32_FLOAT, sizeof(float) * 3, "POSITION");
	fmt.AddElement(DXGI_FORMAT_R32G32_FLOAT, sizeof(float) * 2, "TEXCOORD");
	return fmt;
}


VertexFormat VertPosNormUVColor::GetVertexFormat()
{
	VertexFormat fmt;
	fmt.AddElement(DXGI_FORMAT_R32G32B32_FLOAT, sizeof(float) * 3, "POSITION");
	fmt.AddElement(DXGI_FORMAT_R32G32B32_FLOAT, sizeof(float) * 3, "NORMAL");
	fmt.AddElement(DXGI_FORMAT_R32G32_FLOAT, sizeof(float) * 2, "TEXCOORD");
	fmt.AddElement(DXGI_FORMAT_R8G8B8A8_UNORM, sizeof(unsigned char) * 4, "TEXCOORD", 1);
	return fmt;

}
