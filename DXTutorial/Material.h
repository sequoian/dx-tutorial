#pragma once

#include "Graphics.h"

class Material
{
public:
	~Material();

	bool Load(Graphics& graphics, const wchar_t* shaderFile,
		const VertexFormat& vbfmt);
	void Select(Graphics& graphics);
	void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

private:
	ID3D11VertexShader * m_vs = nullptr;
	ID3D11PixelShader* m_ps = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	D3D11_PRIMITIVE_TOPOLOGY m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
