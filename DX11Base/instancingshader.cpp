#include "pch.h"
#include <io.h>
#include "renderer.h"
#include "instancingshader.h"


void InstancingShader::Init()
{
	auto device = CRenderer::GetDevice();
	auto deviceContext = CRenderer::GetDeviceContext();

	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
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

	// サンプラーステート設定 for shadow map
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	samplerState = NULL;
	device->CreateSamplerState(&samplerDesc, &samplerState);
	deviceContext->PSSetSamplers(1, 1, &samplerState);

	// create vertex shader
	{
		FILE* file;
		long int fsize;

		file = fopen("shader/instancing_vs.cso", "rb");
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

		file = fopen("shader/instancing_ps.cso", "rb");
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

	hBufferDesc.ByteWidth = sizeof(dx::XMFLOAT4);
	device->CreateBuffer(&hBufferDesc, NULL, &m_cameraPosBuffer);

	// structured buffer
	{
		dx::XMFLOAT3* pos = new dx::XMFLOAT3[10000];
		int i = 0;
		for (int x = 0; x < 100; ++x)
		{
			for (int z = 0; z < 100; ++z)
			{
				pos[i] = dx::XMFLOAT3(x, 1.0f, z);
				++i;
			}
		}

		ZeroMemory(&hBufferDesc, sizeof(hBufferDesc));
		hBufferDesc.ByteWidth = sizeof(dx::XMFLOAT3) * 10000;
		hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		hBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		hBufferDesc.StructureByteStride = sizeof(dx::XMFLOAT3);
		hBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = pos;
		CRenderer::GetDevice()->CreateBuffer(&hBufferDesc, &sd, &m_positionBuffer);

		delete[] pos;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_UNKNOWN;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvd.Buffer.FirstElement = 0;
		srvd.Buffer.NumElements = 10000;
		CRenderer::GetDevice()->CreateShaderResourceView(m_positionBuffer, &srvd, &m_positionSRV);
	}

	UpdateConstantBuffers();

	// マテリアル初期化
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1,1,1,1 };
	material.Specular = { 1,1,1,1 };
	SetMaterial(material);
}

void InstancingShader::Uninit()
{
	Shader::Uninit();

	SAFE_RELEASE(m_positionBuffer);
	SAFE_RELEASE(m_positionSRV);
}
