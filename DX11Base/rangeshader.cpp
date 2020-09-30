#include "pch.h"
#include <io.h>
#include "renderer.h"
#include "rangeshader.h"


void RangeShader::Init()
{
	auto device = CRenderer::GetDevice();
	auto deviceContext = CRenderer::GetDeviceContext();

	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* samplerState = NULL;
	device->CreateSamplerState(&samplerDesc, &samplerState);
	deviceContext->PSSetSamplers(0, 1, &samplerState);

	// create vertex shader
	{
		FILE* file;
		long int fsize;

		file = fopen("range_vs.cso", "rb");
		fsize = _filelength(_fileno(file));
		unsigned char* buffer = new unsigned char[fsize];
		fread(buffer, fsize, 1, file);
		fclose(file);

		device->CreateVertexShader(buffer, fsize, NULL, &m_vertexShader);


		// 入力レイアウト生成
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		UINT numElements = ARRAYSIZE(layout);

		device->CreateInputLayout(layout,
			numElements,
			buffer,
			fsize,
			&m_vertexLayout);

		delete[] buffer;
	}

	// create pixel shader
	{
		FILE* file;
		long int fsize;

		file = fopen("range_ps.cso", "rb");
		fsize = _filelength(_fileno(file));
		unsigned char* buffer = new unsigned char[fsize];
		fread(buffer, fsize, 1, file);
		fclose(file);

		device->CreatePixelShader(buffer, fsize, NULL, &m_pixelShader);
	}

	// 定数バッファ生成
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = sizeof(dx::XMMATRIX);
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	device->CreateBuffer(&hBufferDesc, NULL, &m_worldBuffer);
	device->CreateBuffer(&hBufferDesc, NULL, &m_viewBuffer);
	device->CreateBuffer(&hBufferDesc, NULL, &m_projectionBuffer);

	hBufferDesc.ByteWidth = sizeof(MATERIAL);
	device->CreateBuffer(&hBufferDesc, NULL, &m_materialBuffer);

	hBufferDesc.ByteWidth = sizeof(LIGHT);
	device->CreateBuffer(&hBufferDesc, NULL, &m_lightBuffer);

	hBufferDesc.ByteWidth = sizeof(float) * 8;
	device->CreateBuffer(&hBufferDesc, NULL, &m_rangeBuffer);

	hBufferDesc.ByteWidth = sizeof(float) * 4;
	device->CreateBuffer(&hBufferDesc, NULL, &m_timeBuffer);
	device->CreateBuffer(&hBufferDesc, NULL, &m_valueBuffer);

	UpdateConstantBuffers();

	// マテリアル初期化
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// load and set the noise texture
	D3DX11CreateShaderResourceViewFromFile(CRenderer::GetDevice(),
		"asset/texture/noise.png",
		NULL,
		NULL,
		&m_noiseTexture,
		NULL);

	assert(m_noiseTexture);

	PS_SetNoiseTexture(m_noiseTexture);

	// load and set the secondary texture
	D3DX11CreateShaderResourceViewFromFile(CRenderer::GetDevice(),
		"asset/texture/linegrid.jpg",
		NULL,
		NULL,
		&m_secondaryTexture,
		NULL);

	assert(m_secondaryTexture);

	PS_SetSecondaryTexture(m_secondaryTexture);
}

void RangeShader::Uninit()
{
	Shader::Uninit();
	if (m_rangeBuffer) m_rangeBuffer->Release();
}
