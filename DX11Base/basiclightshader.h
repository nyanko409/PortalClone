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
		deviceContext->VSSetConstantBuffers(3, 1, &m_portalBuffer);

		deviceContext->PSSetConstantBuffers(0, 1, &m_lightBuffer);
		deviceContext->PSSetConstantBuffers(1, 1, &m_materialBuffer);
		deviceContext->PSSetConstantBuffers(2, 1, &m_cameraPosBuffer);
	}

	void SetPortalInverseWorldMatrix(bool enableClip, dx::XMMATRIX *inverseWorld = nullptr)
	{
		PortalBuffer buffer;
		if (enableClip)
		{
			buffer.portalInverseWorldBuffer = *inverseWorld;
			buffer.portalInverseWorldBuffer = dx::XMMatrixTranspose(buffer.portalInverseWorldBuffer);
		}

		buffer.enableClip = enableClip;

		CRenderer::GetDeviceContext()->UpdateSubresource(m_portalBuffer, 0, NULL, &buffer, 0, 0);
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

	void SetDirectionalLight(DirectionalLight* Light) override
	{
		CRenderer::GetDeviceContext()->UpdateSubresource(m_lightBuffer, 0, NULL, Light, 0, 0);
	}

	void SetCameraPosition(dx::XMFLOAT3* cameraPosition) override
	{
		dx::XMFLOAT4 cam = dx::XMFLOAT4(cameraPosition->x, cameraPosition->y, cameraPosition->z, 1);
		CRenderer::GetDeviceContext()->UpdateSubresource(m_cameraPosBuffer, 0, NULL, &cam, 0, 0);
	}

	void SetTexture(ID3D11ShaderResourceView* texture) override
	{
		CRenderer::GetDeviceContext()->PSSetShaderResources(0, 1, &texture);
	}

	void SetSamplerState(ID3D11SamplerState* sampler) override
	{
		CRenderer::GetDeviceContext()->PSSetSamplers(0, 1, &sampler);
	}

private:
	struct PortalBuffer
	{
		dx::XMMATRIX portalInverseWorldBuffer;
		int enableClip;
	};

	ID3D11Buffer* m_portalBuffer;
};
