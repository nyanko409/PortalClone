#pragma once

#include "gameObject.h"
#include "rangeshader.h"


class Player : public GameObject
{
private:
	RangeShader* m_shader;
	std::shared_ptr<CModel> m_model;
	float m_moveSpeed;

public:
	Player() {}
	~Player() {}

	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};
