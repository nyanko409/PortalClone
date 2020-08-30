#pragma once


static class CPolygon
{
public:
	static void CreatePlane(float centerX, float centerY, float width, float height, ID3D11Buffer*& vertexBuffer, bool dynamic);
	static void LoadTexture(const char* filename, ID3D11ShaderResourceView*& texture);
};
