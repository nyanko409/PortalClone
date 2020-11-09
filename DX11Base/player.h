#pragma once

#include "gameObject.h"
#include "basiclightshader.h"
#include "obb.h"


class Player : public GameObject
{
public:
	Player() {}
	~Player() {}

	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw(UINT renderPass) override;

	float GetSightRange() const { return m_sightRange; }

private:
	std::shared_ptr<BasicLightShader> m_shader;
	std::shared_ptr<class Model> m_model;

	OBB m_obb;
	bool m_colliding = false;

	float m_moveSpeed;
	float m_sightRange;
	float m_idleYPos = 0;
	dx::XMFLOAT3 m_lookAtDirection;

	void Movement();
	void GetLookAtDirection();
	void ShootProjectile();
};
