#pragma once

#include "gameObject.h"
#include "rangeshader.h"


class Enemy : public GameObject
{
public:
	Enemy() {}
	~Enemy() {}

	void Awake() override;
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void SetMoveSpeed(float moveSpeed) { m_moveSpeed = moveSpeed; }

private:
	RangeShader* m_shader;
	std::shared_ptr<CModel> m_model;
	float m_moveSpeed;
	GameObject* m_rangeObject;
	dx::XMFLOAT3 m_lookDirection;

	void Movement();
};
