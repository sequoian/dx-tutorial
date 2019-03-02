#pragma once

#include "Graphics.h"
#include "Buffer.h"
#include "VertexShader.h"
#include "PixelShader.h"

class Material
{
public:
	static const unsigned int MaxInputs = 32;
	static const unsigned int MaxConstantBuffers = 8;
	static const unsigned int MaxSamplers = 8;

	~Material();

	bool Load(Graphics& graphics, const wchar_t* shaderFile, const VertexFormat& vbfmt);
	void Select(Graphics& graphics);
	void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

	void ClearInputs() { m_numInputs = 0; }
	void AddShaderInput(ID3D11ShaderResourceView* res)
	{
		m_inputs[m_numInputs++] = res;
	}

	void ClearSamplers() { m_numSamplers = 0; }
	void AddShaderSampler(ID3D11SamplerState* sampler)
	{
		m_samplers[m_numSamplers++] = sampler;
	}

	void SetConstantBuffer(unsigned int slot, Buffer& buffer)
	{
		m_cbs[slot] = &buffer;
	}
	void ClearConstantBuffer(unsigned int slot) { m_cbs[slot] = nullptr; }

	void SetShaders(VertexShader* vs, PixelShader* ps)
	{
		m_vs = vs->Get();
		m_ps = ps->Get();
	}


private:
	ID3D11VertexShader * m_vs = nullptr;
	ID3D11PixelShader* m_ps = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	D3D11_PRIMITIVE_TOPOLOGY m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	const Buffer* m_cbs[MaxConstantBuffers] = {};
	ID3D11ShaderResourceView* m_inputs[MaxInputs] = {};
	ID3D11SamplerState* m_samplers[MaxSamplers] = {};
	unsigned int m_numInputs = 0;
	unsigned int m_numSamplers = 0;

};
