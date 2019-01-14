#include "Material.h"


Material::~Material()
{
	if (m_vs != nullptr)
	{
		m_vs->Release();
		m_vs = nullptr;
	}

	if (m_ps != nullptr)
	{
		m_ps->Release();
		m_ps = nullptr;
	}

	if (m_layout != nullptr)
	{
		m_layout->Release();
		m_layout = nullptr;
	}
}


bool Material::Load(Graphics& graphics, const wchar_t* shaderFile, const VertexFormat& vbfmt)
{
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;

	// Create vertex shader
	vsBlob = graphics.CreateShaderFromFile(shaderFile, "vsmain", "vs_5_0");
	if (!vsBlob)
	{
		return false;
	}

	m_vs = graphics.CreateVertexShader(vsBlob);
	if (!m_vs)
	{
		return false;
	}
	
	// Create pixel shader
	psBlob = graphics.CreateShaderFromFile(shaderFile, "psmain", "ps_5_0");
	if (!psBlob)
	{
		return false;
	}

	m_ps = graphics.CreatePixelShader(psBlob);
	if (!m_ps)
	{
		return false;
	}

	// Create the input layout
	m_layout = graphics.CreateInputLayout(vsBlob, vbfmt);
	if (!m_layout)
	{
		return false;
	}

	// Release the blobs!
	vsBlob->Release();
	psBlob->Release();

	return true;
}


void Material::Select(Graphics& graphics)
{
	graphics.SetPrimitiveTopology(m_topology);
	graphics.SetInputLayout(m_layout);
	graphics.SetVertexShader(m_vs);
	graphics.SetPixelShader(m_ps);

	ID3D11Buffer* buff = m_cbs[0]->GetCurrentBuffer();
	graphics.SetVSConstantBuffers(0, 1, &buff);
	graphics.SetPSConstantBuffers(0, 1, &buff);
	graphics.SetVSShaderInputs(0, 1, m_inputs);
	graphics.SetPSShaderInputs(0, 1, m_inputs);
	graphics.SetVSSamplers(0, 1, m_samplers);
	graphics.SetPSSamplers(0, 1, m_samplers);
}


void Material::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	m_topology = topology;
}