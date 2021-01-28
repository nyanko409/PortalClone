#pragma once

#include "portal.h"
#include "rendertexture.h"
#include "portalrendertextureshader.h"


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
	dx::XMMATRIX GetViewMatrix(bool firstIteration) override;

private:
	std::shared_ptr<PortalRenderTextureShader> m_shader;

	std::weak_ptr<RenderTexture> m_renderTexture;
	std::weak_ptr<RenderTexture> m_tempRenderTexture;
	std::weak_ptr<RenderTexture> m_activeRenderTexture;

	int m_iterationNum;

	void SetupNextIteration();
};
