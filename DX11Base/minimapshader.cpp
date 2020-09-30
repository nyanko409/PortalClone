#include "pch.h"
#include <io.h>
#include "renderer.h"
#include "minimapshader.h"
#include "main.h"


void MinimapShader::Init()
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

		file = fopen("minimap_vs.cso", "rb");
		fsize = _filelength(_fileno(file));
		unsigned char* buffer = new unsigned char[fsize];
		fread(buffer, fsize, 1, file);
		fclose(file);

		device->CreateVertexShader(buffer, fsize, NULL, &m_vertexShader);


		// 入力レイアウト生成
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 10, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

		file = fopen("minimap_ps.cso", "rb");
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

	UpdateConstantBuffers();

	// マテリアル初期化
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// init minimap camera
	dx::XMMATRIX projection = dx::XMMatrixPerspectiveFovLH(1.0F, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1F, 1000.0F);
	dx::XMStoreFloat4x4(&m_mProjection, projection);

	dx::XMMATRIX view;
	dx::XMVECTOR eye = dx::XMVectorSet(0, 20, 0, 1);
	dx::XMVECTOR forward = dx::XMVectorSet(0, -1, 0.001F, 1);

	dx::XMFLOAT3 fup = dx::XMFLOAT3(0, 1, 0);
	dx::XMVECTOR up = dx::XMLoadFloat3(&fup);

	view = dx::XMMatrixLookAtLH(eye, dx::XMVectorAdd(eye, forward), up);
	dx::XMStoreFloat4x4(&m_mView, view);
}

void MinimapShader::Uninit()
{
	Shader::Uninit();
}
