#pragma once

#include "gameObject.h"
#include "basiclightshader.h"


class Billboard : public GameObject
{
private:
	BasicLightShader* m_shader;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11ShaderResourceView* m_texture;
	int m_count;

public:
	void Awake() override;
	void Uninit() override;
	void Draw() override;
	void Update() override;
};
