#pragma once

#include "gameObject.h"
#include "portalshader.h"
#include "rendertexture.h"
#include "obbcollider.h"


enum class PortalType
{
	Blue, Orange
};

class Portal : public GameObject
{
	friend class PortalManager;

public:
	Portal() {}
	~Portal() {}

	void Awake() override;
	void Uninit() override;
	void Update() override;
	void Draw(Pass pass) override;
	void Draw(const std::shared_ptr<class Shader>& shader, Pass pass) override;

	void SetLookAt(dx::XMFLOAT3 lookAt) { m_lookAt = lookAt; }
	void SetUp(dx::XMFLOAT3 up) { m_up = up; }
	void SetColor(dx::XMFLOAT4 color) { m_color = color; }
	void SetRenderTexture(const std::shared_ptr<RenderTexture>& renderTexture) { m_renderTexture = renderTexture; }
	void SetTempRenderTexture(const std::shared_ptr<RenderTexture>& renderTexture) { m_tempRenderTexture = renderTexture; }
	void IsOtherPortalActive(bool active) { m_linkedPortalActive = active; }
	void SetOtherPortal(const std::shared_ptr<Portal>& otherPortal) { m_linkedPortal = otherPortal; }
	void SetRecursionNum(uint32_t num) { m_iterationNum = num; }
	void SetType(PortalType type) { m_type = type; }
	OBB* GetObb() { return &m_obb; }

	dx::XMMATRIX GetViewMatrix(bool firstIteration = false);
	dx::XMMATRIX GetProjectionMatrix();
	dx::XMMATRIX GetLocalToWorldMatrix();
	dx::XMMATRIX GetWorldToLocalMatrix();
	dx::XMMATRIX GetPlayerOrientationMatrix(dx::XMFLOAT3 position);
	dx::XMMATRIX GetPlayerWorldMatrix(dx::XMFLOAT3 position);
	PortalType GetType() const { return m_type; }

private:
	std::shared_ptr<PortalShader> m_shader;
	std::shared_ptr<class Model> m_model;
	OBB m_obb;

	std::weak_ptr<RenderTexture> m_renderTexture;
	std::weak_ptr<RenderTexture> m_tempRenderTexture;
	std::weak_ptr<RenderTexture> m_activeRenderTexture;

	std::weak_ptr<Portal> m_linkedPortal;
	PortalType m_type;
	bool m_linkedPortalActive = false;

	dx::XMFLOAT3 m_lookAt;
	dx::XMFLOAT3 m_up;
	dx::XMFLOAT4 m_color;

	int m_iterationNum, m_curIteration;

	void SetupNextIteration();
};
