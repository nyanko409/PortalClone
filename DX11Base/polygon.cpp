#include "pch.h"
#include "main.h"
#include "renderer.h"
#include "polygon.h"


void CPolygon::CreatePlaneCenter(float centerX, float centerY, float width, float height, ID3D11Buffer*& vertexBuffer, bool dynamic)
{
	VERTEX_3D vertex[4];
	float halfWidth = width / 2;
	float halfHeight = height / 2;

	vertex[0].Position = dx::XMFLOAT3(centerX - halfWidth, centerY - halfHeight, 0.0f);
	vertex[0].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = dx::XMFLOAT2(0.0f, 0.0f);

	vertex[1].Position = dx::XMFLOAT3(centerX + halfWidth, centerY - halfHeight, 0.0f);
	vertex[1].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[1].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = dx::XMFLOAT2(1.0f, 0.0f);

	vertex[2].Position = dx::XMFLOAT3(centerX - halfWidth, centerY + halfHeight, 0.0f);
	vertex[2].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[2].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = dx::XMFLOAT2(0.0f, 1.0f);

	vertex[3].Position = dx::XMFLOAT3(centerX + halfWidth, centerY + halfHeight, 0.0f);
	vertex[3].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[3].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = dx::XMFLOAT2(1.0f, 1.0f);

	//頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (dynamic)
	{
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0;
	}

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &vertexBuffer);
}

void CPolygon::CreatePlaneTopLeft(float topLeftX, float topLeftY, float width, float height, ID3D11Buffer*& vertexBuffer, bool dynamic)
{
	VERTEX_3D vertex[4];

	vertex[0].Position = dx::XMFLOAT3(topLeftX, topLeftY, 0.0f);
	vertex[0].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = dx::XMFLOAT2(0.0f, 0.0f);

	vertex[1].Position = dx::XMFLOAT3(topLeftX + width, topLeftY, 0.0f);
	vertex[1].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[1].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = dx::XMFLOAT2(1.0f, 0.0f);

	vertex[2].Position = dx::XMFLOAT3(topLeftX, topLeftY + height, 0.0f);
	vertex[2].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[2].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = dx::XMFLOAT2(0.0f, 1.0f);

	vertex[3].Position = dx::XMFLOAT3(topLeftX + width, topLeftY + height, 0.0f);
	vertex[3].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[3].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = dx::XMFLOAT2(1.0f, 1.0f);

	//頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (dynamic)
	{
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0;
	}

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &vertexBuffer);
}

void CPolygon::RemapDimensionsTopLeft(float topLeftX, float topLeftY, float width, float height, ID3D11Buffer*& vertexBuffer)
{
	try
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		CRenderer::GetDeviceContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		if (!vertex)
			throw std::exception();

		vertex[0].Position = dx::XMFLOAT3(topLeftX, topLeftY, 0.0f);
		vertex[0].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertex[0].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = dx::XMFLOAT2(0.0f, 0.0f);

		vertex[1].Position = dx::XMFLOAT3(topLeftX + width, topLeftY, 0.0f);
		vertex[1].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertex[1].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = dx::XMFLOAT2(1.0f, 0.0f);

		vertex[2].Position = dx::XMFLOAT3(topLeftX, topLeftY + height, 0.0f);
		vertex[2].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertex[2].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = dx::XMFLOAT2(0.0f, 1.0f);

		vertex[3].Position = dx::XMFLOAT3(topLeftX + width, topLeftY + height, 0.0f);
		vertex[3].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertex[3].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = dx::XMFLOAT2(1.0f, 1.0f);

		CRenderer::GetDeviceContext()->Unmap(vertexBuffer, 0);
	}
	catch (const std::exception&)
	{
		MessageBox(GetWindow(), "buffer description is not set up properly!", NULL, MB_OK);
		exit(0);
	}
}

void CPolygon::LoadTexture(const char* filename, ID3D11ShaderResourceView*& texture)
{
	//テクスチャ読み込み
	SAFE_RELEASE(texture);
	D3DX11CreateShaderResourceViewFromFile(CRenderer::GetDevice(), filename, NULL, NULL, &texture, NULL);
	assert(texture);
}
