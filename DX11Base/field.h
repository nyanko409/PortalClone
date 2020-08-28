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

	float m_width, m_height;

public:
	Field() {}
	~Field() {}

	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	// if collided with x axis, the first bit is set (0b0001)
	// if collided with z axis, the second bit is set (0b0010)
	int CheckBounds(dx::XMFLOAT3 position, float objectRadius);
};
