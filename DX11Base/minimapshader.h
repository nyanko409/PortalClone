#pragma once

#include "shader.h"


class MinimapShader : public Shader
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
		deviceContext->VSSetConstantBuffers(0, 1, &m_projectionBuffer);
		deviceContext->VSSetConstantBuffers(1, 1, &m_materialBuffer);

		deviceContext->UpdateSubresource(m_projectionBuffer, 0, NULL, &projection, 0, 0);
	}

	void PS_SetMinimapTexture(ID3D11ShaderResourceView* texture)
	{
		CRenderer::GetDeviceContext()->PSSetShaderResources(1, 1, &texture);
	}

	void SetMaterial(MATERIAL Material) override
	{
		CRenderer::GetDeviceContext()->UpdateSubresource(m_materialBuffer, 0, NULL, &Material, 0, 0);
	}

	void PS_SetTexture(ID3D11ShaderResourceView* texture) override
	{
		CRenderer::GetDeviceContext()->PSSetShaderResources(0, 1, &texture);
	}

private:
	dx::XMMATRIX projection;
	ID3D11ShaderResourceView* m_minimapTexture;
};
