#pragma once

#include "gameObject.h"
#include "collision.h"


enum class PortalType
{
	None, Blue, Orange
};

class Portal : public GameObject
{
public:
	virtual void Awake() override;

	// setters
	void SetColor(dx::XMFLOAT4 color) { m_color = color; }
	void SetOtherPortal(const std::shared_ptr<Portal>& otherPortal) { m_linkedPortal = otherPortal; }
	void SetType(PortalType type) { m_type = type; }
	void SetAttachedColliderId(int id) { m_attachedColliderId = id; }

	// used for rendering with render texture
	virtual void SetRenderTexture(const std::shared_ptr<RenderTexture>& renderTexture) {}
	virtual void SetTempRenderTexture(const std::shared_ptr<RenderTexture>& renderTexture) {}
	virtual void SetRecursionNum(uint32_t num) {}

	// getters
	virtual dx::XMMATRIX GetViewMatrix(bool firstIteration = false) = 0;
	dx::XMMATRIX GetProjectionMatrix();
	dx::XMVECTOR GetClonedVelocity(dx::XMVECTOR velocity) const;
	dx::XMVECTOR GetClonedPosition(dx::XMVECTOR position) const;
	dx::XMMATRIX GetClonedOrientationMatrix(dx::XMMATRIX matrix) const;
	std::shared_ptr<Portal> GetLinkedPortal() const { return m_linkedPortal.lock(); }

	PortalType GetType() const { return m_type; }
	OBB* GetTriggerCollider() { return &m_triggerCollider; }
	std::vector<OBB*>* GetEdgeColliders() { return &m_edgeColliders; }
	int GetAttachedColliderId() const { return m_attachedColliderId; }

protected:
	std::shared_ptr<class Model> m_model;
	OBB m_triggerCollider;
	std::vector<OBB*> m_edgeColliders;
	int m_attachedColliderId;

	std::weak_ptr<Portal> m_linkedPortal;
	PortalType m_type;

	dx::XMFLOAT4 m_color;

	int m_curIteration;
};
