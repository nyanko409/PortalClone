#pragma once

#include "gameObject.h"
#include "portalstencilshader.h"
#include "rendertexture.h"
#include "collision.h"


enum class PortalStencilType
{
	None, Blue, Orange
};

class PortalStencil : public GameObject
{
	friend class PortalStencilManager;

public:
	PortalStencil() {}
	~PortalStencil() {}

	// overrides
	void Awake() override;
	void Uninit() override;
	void Update() override;
	void Draw(Pass pass) override;
	void Draw(const std::shared_ptr<class Shader>& shader, Pass pass) override;

	// setters
	void SetColor(dx::XMFLOAT4 color) { m_color = color; }
	void SetOtherPortalActive(bool active) { m_linkedPortalActive = active; }
	void SetOtherPortal(const std::shared_ptr<PortalStencil>& otherPortal) { m_linkedPortal = otherPortal; }
	void SetRecursionNum(uint32_t num) { m_iterationNum = num; }
	void SetType(PortalStencilType type) { m_type = type; }
	void SetAttachedColliderId(int id) { m_attachedColliderId = id; }

	// getters
	dx::XMMATRIX GetViewMatrix(bool firstIteration = false);
	dx::XMMATRIX GetProjectionMatrix();

	PortalStencilType GetType() const { return m_type; }
	OBB* GetTriggerCollider() { return &m_triggerCollider; }
	std::vector<OBB*>* GetEdgeColliders() { return &m_edgeColliders; }
	int GetAttachedColliderId() const { return m_attachedColliderId; }

	dx::XMVECTOR GetClonedVelocity(dx::XMVECTOR velocity) const;
	dx::XMMATRIX GetClonedOrientationMatrix(dx::XMMATRIX matrix) const;

private:
	std::shared_ptr<PortalStencilShader> m_shader;
	std::shared_ptr<class Model> m_model;
	OBB m_triggerCollider;
	std::vector<OBB*> m_edgeColliders;
	int m_attachedColliderId;

	std::weak_ptr<PortalStencil> m_linkedPortal;
	PortalStencilType m_type;
	bool m_linkedPortalActive = false;

	dx::XMFLOAT4 m_color;

	int m_iterationNum, m_curIteration;
};
