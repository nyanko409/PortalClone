#pragma once

#include "shader.h"


class RangeShader : public Shader
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

		deviceContext->PSSetConstantBuffers(0, 1, &m_rangeBuffer);
		deviceContext->PSSetConstantBuffers(1, 1, &m_timeBuffer);
		deviceContext->PSSetConstantBuffers(2, 1, &m_lightBuffer);

		m_noiseValue += 0.001F;
		PS_SetTimeBuffer(m_noiseValue);
	}

	void PS_SetRangeBuffer(const float range, const dx::XMFLOAT3& playerPos)
	{
		Range r;
		r.range = range;
		r.playerPos[0] = playerPos.x;
		r.playerPos[1] = playerPos.y;
		r.playerPos[2] = playerPos.z;

		CRenderer::GetDeviceContext()->UpdateSubresource(m_rangeBuffer, 0, NULL, &r, 0, 0);
	}

	void PS_SetRangeBuffer(const float range, const dx::XMVECTOR& playerPos)
	{
		Range r;
		r.range = range;
		r.playerPos[0] = dx::XMVectorGetX(playerPos);
		r.playerPos[1] = dx::XMVectorGetY(playerPos);
		r.playerPos[2] = dx::XMVectorGetZ(playerPos);

		CRenderer::GetDeviceContext()->UpdateSubresource(m_rangeBuffer, 0, NULL, &r, 0, 0);
	}

	void PS_SetNoiseTexture(ID3D11ShaderResourceView* texture)
	{
		CRenderer::GetDeviceContext()->PSSetShaderResources(1, 1, &texture);
	}

	void PS_SetNormalTexture(ID3D11ShaderResourceView* texture)
	{
		CRenderer::GetDeviceContext()->PSSetShaderResources(2, 1, &texture);
	}

	void PS_SetTimeBuffer(const float time)
	{
		CRenderer::GetDeviceContext()->UpdateSubresource(m_timeBuffer, 0, NULL, &time, 0, 0);
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

	void PS_SetTexture(ID3D11ShaderResourceView* texture) override
	{
		CRenderer::GetDeviceContext()->PSSetShaderResources(0, 1, &texture);
	}

private:
	ID3D11Buffer* m_rangeBuffer;
	ID3D11Buffer* m_timeBuffer;
	ID3D11ShaderResourceView* m_noiseTexture;

	float m_noiseValue;

	struct Range
	{
		float range;
		float playerPos[3];
	};

	struct Time
	{
		float time;
		float dummy[3];
	};
};
