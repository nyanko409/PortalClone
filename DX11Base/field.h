#pragma once

#include "gameObject.h"
#include "basiclightshader.h"


class Field : public GameObject
{
private:
	BasicLightShader* m_shader = nullptr;
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11ShaderResourceView* m_Texture = nullptr;

public:
	Field() {}
	~Field() {}

	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};
