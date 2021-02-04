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

	void Swap() override;
	dx::XMVECTOR GetTravelerPosition() const override;

	void InTitleDisplayMode(bool mode) { m_titleDisplay = mode; }

private:
	std::shared_ptr<BasicLightShader> m_shader;
	std::shared_ptr<class Model> m_model;
	std::shared_ptr<class FPSCamera> m_camera;

	std::weak_ptr<GameObject> m_grabbingObject;

	float m_moveSpeed;
	dx::XMFLOAT3 m_movementVelocity;
	bool m_isJumping;
	bool m_titleDisplay;

	void UpdateAnimation();
	void Movement();
	void Jump();
	void UpdateCollision();
	void ShootPortal(PortalType type);
	void GrabObject();
	void UpdateGrabObject();
	void UpdateGrabCollision();
	dx::XMMATRIX GetFixedUpWorldMatrix() const;
	dx::XMMATRIX GetClonedWorldMatrix() const;
};
