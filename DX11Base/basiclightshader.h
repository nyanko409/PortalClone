#pragma once

#include "shader.h"


class BasicLightShader : public Shader
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
		deviceContext->VSSetConstantBuffers(3, 1, &m_materialBuffer);
		deviceContext->VSSetConstantBuffers(4, 1, &m_lightBuffer);
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

	void SetMaterial(MATERIAL Material) override
	{
		CRenderer::GetDeviceContext()->UpdateSubresource(m_materialBuffer, 0, NULL, &Material, 0, 0);
	}

	void SetLight(LIGHT Light) override
	{
		CRenderer::GetDeviceContext()->UpdateSubresource(m_lightBuffer, 0, NULL, &Light, 0, 0);
	}

	void SetTexture(ID3D11ShaderResourceView* texture) override
	{
		CRenderer::GetDeviceContext()->PSSetShaderResources(0, 1, &texture);
	}

	void SetSamplerState(ID3D11SamplerState* sampler) override
	{
		CRenderer::GetDeviceContext()->PSSetSamplers(0, 1, &sampler);
	}
};
