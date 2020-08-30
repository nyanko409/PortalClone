#pragma once


static class CPolygon
{
public:
	static void CreatePlaneCenter(float centerX, float centerY, float width, float height, ID3D11Buffer*& vertexBuffer, bool dynamic);
	static void CreatePlaneTopLeft(float topLeftX, float topLeftY, float width, float height, ID3D11Buffer*& vertexBuffer, bool dynamic);
	static void LoadTexture(const char* filename, ID3D11ShaderResourceView*& texture);

	static void RemapDimensionsTopLeft(float topLeftX, float topLeftY, float width, float height, ID3D11Buffer*& vertexBuffer);
};
