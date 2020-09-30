#pragma once

#include "gameobject.h"
#include "rangeshader.h"
#include "minimapshader.h"
#include "basiclightshader.h"


class Terrain : public GameObject
{
public:
	void Awake() override;
	void Uninit() override;
	void Update() override;
	void Draw(UINT renderPass) override;

	void CreateTerrain(int size);
	float GetHeight(dx::XMFLOAT3 position);

private:
	VERTEX_3D** m_vertices;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	ID3D11ShaderResourceView* m_texture;

	std::shared_ptr<RangeShader> m_shader;
	std::shared_ptr<MinimapShader> m_minimapShader;

	UINT m_indexCount;
	UINT m_size = 0;
};