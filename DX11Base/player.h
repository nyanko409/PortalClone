#pragma once

#include "gameObject.h"
#include "basiclightshader.h"
#include "obb.h"
#include "portalmanager.h"


class Player : public GameObject
{
public:
	Player() {}
	~Player() {}

	void Awake() override;
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw(Pass pass) override;
	void Draw(const std::shared_ptr<Shader>& shader, Pass pass) override;

	void InTitleDisplayMode(bool mode) { m_titleDisplay = mode; }

private:
	std::shared_ptr<BasicLightShader> m_shader;
	std::shared_ptr<class Model> m_model;

	OBB m_obb;
	dx::XMFLOAT3 m_intersectVector;

	float m_moveSpeed;
	dx::XMFLOAT3 m_velocity;
	dx::XMFLOAT3 m_lookAtDirection;
	bool m_isJumping;
	bool m_titleDisplay;

	void Movement();
	void Jump();
	void ShootPortal(PortalType type);
	dx::XMMATRIX GetAdjustedWorldMatrix();
};
