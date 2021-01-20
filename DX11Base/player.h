#pragma once

#include "gameObject.h"
#include "basiclightshader.h"
#include "collision.h"
#include "portalmanager.h"
#include "portaltraveler.h"


class Player : public GameObject, public PortalTraveler
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

	void InTitleDisplayMode(bool mode) { m_titleDisplay = mode; }

	OBB* GetOBB() override { return &m_obb; }
	void SetEntrancePortal(PortalType type) override { m_entrancePortal = type; }
	PortalType GetEntrancePortal() const override { return m_entrancePortal; }
	void Swap() override;

private:
	std::shared_ptr<BasicLightShader> m_shader;
	std::shared_ptr<class Model> m_model;
	std::shared_ptr<class FPSCamera> m_camera;

	OBB m_obb;
	PortalType m_entrancePortal;

	float m_moveSpeed;
	dx::XMFLOAT3 m_velocity, m_movementVelocity;
	bool m_isJumping;
	bool m_titleDisplay;
	dx::XMFLOAT3 m_clonedPos, m_clonedForward, m_clonedUp;

	void UpdateAnimation();
	void Movement();
	void Jump();
	void UpdateCollision();
	void ShootPortal(PortalType type);
	dx::XMMATRIX GetFixedUpWorldMatrix() const;
	dx::XMMATRIX GetClonedWorldMatrix() const;
};
