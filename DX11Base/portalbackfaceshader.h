#pragma once

#include "shader.h"


class PortalBackfaceShader : public Shader
{
public:
	void Init() override;
	void Uninit() override;

	void UpdateConstantBuffers() override
	{
		auto deviceContext = CRenderer::GetDeviceContext();

		// set input layout
		deviceContext->IASetInputLayout(m_vertexLayout);

		// set constant buffers
		deviceContext->VSSetConstantBuffers(0, 1, &m_worldBuffer);
		deviceContext->VSSetConstantBuffers(1, 1, &m_viewBuffer);
		deviceContext->VSSetConstantBuffers(2, 1, &m_projectionBuffer);

		deviceContext->PSSetConstantBuffers(0, 1, &m_materialBuffer);
		deviceContext->PSSetConstantBuffers(1, 1, &m_valueBuffer);
		deviceContext->PSSetShaderResources(0, 1, &m_maskTexture);
	}

	void SetValueBuffer(int stencilPass)
	{
		CRenderer::GetDeviceContext()->UpdateSubresource(m_valueBuffer, 0, NULL, &stencilPass, 0, 0);
	}

	void SetMaterial(MATERIAL material) override
	{
		CRenderer::GetDeviceContext()->UpdateSubresource(m_materialBuffer, 0, NULL, &material, 0, 0);
	}

	void SetWorldMatrix(dx::XMMATRIX *WorldMatrix) override
	{
		dx::XMMATRIX world = *WorldMatrix;
		world = dx::XMMatrixTranspose(world);
		CRenderer::GetDeviceContext()->UpdateSubresource(m_worldBuffer, 0, NULL, &world, 0, 0);
	}

	void SetViewMatrix(dx::XMMATRIX *ViewMatrix) override
	{
		dx::XMMATRIX view = *ViewMatrix;
		view = dx::XMMatrixTranspose(view);
		CRenderer::GetDeviceContext()->UpdateSubresource(m_viewBuffer, 0, NULL, &view, 0, 0);
	}

	void SetProjectionMatrix(dx::XMMATRIX *ProjectionMatrix) override
	{
		dx::XMMATRIX projection = *ProjectionMatrix;
		projection = dx::XMMatrixTranspose(projection);
		CRenderer::GetDeviceContext()->UpdateSubresource(m_projectionBuffer, 0, NULL, &projection, 0, 0);
	}

private:
	ID3D11Buffer* m_valueBuffer;
	ID3D11ShaderResourceView* m_maskTexture;
};
