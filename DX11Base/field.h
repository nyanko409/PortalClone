#pragma once

#include "gameObject.h"
#include "rangeshader.h"


class Field : public GameObject
{
private:
	RangeShader* m_shader = nullptr;
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11ShaderResourceView* m_Texture = nullptr;
	GameObject* m_rangeObject = nullptr;

public:
	Field() {}
	~Field() {}

	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};
