#pragma once

#include "gameObject.h"
#include "rangeshader.h"
#include "model.h"


class Field : public GameObject
{
private:
	RangeShader* m_shader;
	std::shared_ptr<CModel> m_model;
	ID3D11ShaderResourceView* m_normalTexture;
	std::weak_ptr<GameObject> m_rangeObject;

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
