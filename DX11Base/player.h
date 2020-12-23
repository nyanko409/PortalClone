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
	OBB* GetObb() { return &m_obb; }

	void SetEntrancePortal(const std::shared_ptr<Portal>& portal) { m_entrancePortal = portal; }
	std::weak_ptr<Portal> GetEntrancePortal() const { return m_entrancePortal; }
	void UnsetEntrancePortal() { m_entrancePortal.reset(); }
	void SwapPosition() { SetPosition(m_clonedPos); }

private:
	std::shared_ptr<BasicLightShader> m_shader;
	std::shared_ptr<class Model> m_model;
	std::shared_ptr<class Model> m_portalGun;

	OBB m_obb;
	std::weak_ptr<Portal> m_entrancePortal;

	float m_moveSpeed;
	dx::XMFLOAT3 m_velocity;
	dx::XMFLOAT3 m_lookAtDirection;
	bool m_isJumping;
	bool m_titleDisplay;
	dx::XMFLOAT3 m_clonedPos;

	void Movement();
	void Jump();
	void ShootPortal(PortalType type);
	dx::XMMATRIX GetAdjustedWorldMatrix();
	dx::XMMATRIX GetPortalGunWorldMatrix();

	dx::XMMATRIX GetClonedWorldMatrix();
	dx::XMMATRIX GetClonedPortalGunWorldMatrix();
};
