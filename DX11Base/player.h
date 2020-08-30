#pragma once

#include "gameObject.h"
#include "basiclightshader.h"


class Player : public GameObject
{
public:
	Player() {}
	~Player() {}

	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

private:
	BasicLightShader* m_shader;
	std::shared_ptr<CModel> m_model;
	float m_moveSpeed;
	float m_idleYPos = 0;
	dx::XMFLOAT3 m_lookAtDirection;

	void Movement();
	void GetLookAtDirection();
	void IdleAnimation();
	void ShootProjectile();
};
