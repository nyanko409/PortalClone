#pragma once

#include "gameobject.h"
#include "basiclightshader.h"


class Terrain : public GameObject
{
public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	ID3D11ShaderResourceView* m_texture;

	std::shared_ptr<BasicLightShader> m_shader;

	UINT m_indexCount;
};