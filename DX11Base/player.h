#pragma once

#include "gameObject.h"
#include "basiclightshader.h"
#include "collision.h"
#include "portalstencilmanager.h"
#include "portalstenciltraveler.h"


class Player : public GameObject, public PortalStencilTraveler
{
public:
	Player() {}
	~Player() {}

	dx::XMFLOAT3 virtualUp;

	void Awake() override;
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw(Pass pass) override;
	void Draw(const std::shared_ptr<Shader>& shader, Pass pass) override;

	dx::XMMATRIX GetWorldMatrix() const override;

	void Swap() override;
	dx::XMVECTOR GetTravelerPosition() const override;

	void InTitleDisplayMode(bool mode) { m_titleDisplay = mode; }

private:
	std::shared_ptr<BasicLightShader> m_shader;
	std::shared_ptr<class Model> m_model;
	std::shared_ptr<class FPSCamera> m_camera;

	float m_moveSpeed;
	dx::XMFLOAT3 m_velocity, m_movementVelocity;
	bool m_isJumping;
	bool m_titleDisplay;

	void UpdateAnimation();
	void Movement();
	void Jump();
	void UpdateCollision();
	void ShootPortal(PortalStencilType type);
	dx::XMMATRIX GetFixedUpWorldMatrix() const;
	dx::XMMATRIX GetClonedWorldMatrix() const;
};
