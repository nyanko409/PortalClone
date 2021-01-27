#pragma once

#include "portal.h"
#include "portalstencilshader.h"
#include "rendertexture.h"


class PortalStencil : public Portal
{
public:
	PortalStencil() {}
	~PortalStencil() {}

	void Awake() override;
	void Uninit() override;
	void Update() override;
	void Draw(Pass pass) override;
	void Draw(const std::shared_ptr<class Shader>& shader, Pass pass) override;

	// getters
	dx::XMMATRIX GetViewMatrix(bool firstIteration = false) override;
	dx::XMMATRIX GetProjectionMatrix() override;

	dx::XMVECTOR GetClonedVelocity(dx::XMVECTOR velocity) const override;
	dx::XMMATRIX GetClonedOrientationMatrix(dx::XMMATRIX matrix) const override;

private:
	std::shared_ptr<PortalStencilShader> m_shader;
};
