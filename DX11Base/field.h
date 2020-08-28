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

	float m_colDepth, m_colWidth;

public:
	Field() {}
	~Field() {}

	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	bool IsOutOfBounds(dx::XMFLOAT3 position, float objectRadius);
};
