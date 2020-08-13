#pragma once

#include "renderer.h"


class Shader
{
public:
	virtual void Init() = 0;
	virtual void Uninit()
	{
		m_worldBuffer->Release();
		m_viewBuffer->Release();
		m_projectionBuffer->Release();
		m_lightBuffer->Release();
		m_materialBuffer->Release();

		m_vertexLayout->Release();
		m_vertexShader->Release();
		m_pixelShader->Release();
	}

	void SetWorldMatrix(dx::XMMATRIX *WorldMatrix)
	{
		dx::XMMATRIX world = *WorldMatrix;
		world = dx::XMMatrixTranspose(world);
		CRenderer::GetDeviceContext()->UpdateSubresource(m_worldBuffer, 0, NULL, &world, 0, 0);
	}

	void SetViewMatrix(dx::XMMATRIX *ViewMatrix)
	{
		dx::XMMATRIX view = *ViewMatrix;
		view = dx::XMMatrixTranspose(view);
		CRenderer::GetDeviceContext()->UpdateSubresource(m_viewBuffer, 0, NULL, &view, 0, 0);
	}

	void SetProjectionMatrix(dx::XMMATRIX *ProjectionMatrix)
	{
		dx::XMMATRIX projection = *ProjectionMatrix;
		projection = dx::XMMatrixTranspose(projection);
		CRenderer::GetDeviceContext()->UpdateSubresource(m_projectionBuffer, 0, NULL, &projection, 0, 0);
	}

	void SetMaterial(MATERIAL Material)
	{
		CRenderer::GetDeviceContext()->UpdateSubresource(m_materialBuffer, 0, NULL, &Material, 0, 0);
	}

	void SetLight(LIGHT Light)
	{
		CRenderer::GetDeviceContext()->UpdateSubresource(m_lightBuffer, 0, NULL, &Light, 0, 0);
	}

	void PS_SetTexture(ID3D11ShaderResourceView* texture)
	{
		CRenderer::GetDeviceContext()->PSSetShaderResources(0, 1, &texture);
	}

protected:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_vertexLayout;

	ID3D11Buffer* m_worldBuffer;
	ID3D11Buffer* m_viewBuffer;
	ID3D11Buffer* m_projectionBuffer;
	ID3D11Buffer* m_materialBuffer;
	ID3D11Buffer* m_lightBuffer;

	friend class CRenderer;
};
