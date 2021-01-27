#pragma once

#include "portal.h"
#include "portalshader.h"
#include "rendertexture.h"


class PortalRenderTexture : public Portal
{
public:
	PortalRenderTexture() {}
	~PortalRenderTexture() {}

	void Awake() override;
	void Uninit() override;
	void Update() override;
	void Draw(Pass pass) override;
	void Draw(const std::shared_ptr<class Shader>& shader, Pass pass) override;

	// setters
	void SetRenderTexture(const std::shared_ptr<RenderTexture>& renderTexture) override { m_renderTexture = renderTexture; }
	void SetTempRenderTexture(const std::shared_ptr<RenderTexture>& renderTexture) override { m_tempRenderTexture = renderTexture; }
	void SetRecursionNum(uint32_t num) override { m_iterationNum = num; }

	// getters
	dx::XMMATRIX GetViewMatrix(bool firstIteration = false) override;
	dx::XMMATRIX GetProjectionMatrix() override;

	dx::XMVECTOR GetClonedVelocity(dx::XMVECTOR velocity) const override;
	dx::XMMATRIX GetClonedOrientationMatrix(dx::XMMATRIX matrix) const override;

private:
	std::shared_ptr<PortalShader> m_shader;

	std::weak_ptr<RenderTexture> m_renderTexture;
	std::weak_ptr<RenderTexture> m_tempRenderTexture;
	std::weak_ptr<RenderTexture> m_activeRenderTexture;

	int m_iterationNum;

	void SetupNextIteration();
};
