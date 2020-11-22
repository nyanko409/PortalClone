#pragma once

#include "renderer.h"
#include "light.h"


class Shader
{
	friend class CRenderer;

public:
	virtual void Init() = 0;
	virtual void Uninit()
	{
		if (m_worldBuffer) m_worldBuffer->Release();
		if (m_viewBuffer) m_viewBuffer->Release();
		if (m_projectionBuffer) m_projectionBuffer->Release();
		if (m_lightBuffer) m_lightBuffer->Release();
		if (m_materialBuffer) m_materialBuffer->Release();

		if (m_vertexLayout) m_vertexLayout->Release();
		if (m_vertexShader) m_vertexShader->Release();
		if (m_pixelShader) m_pixelShader->Release();
	}

	virtual void UpdateConstantBuffers() = 0;

	// if needed, override these methods in the shader class it derives from
	virtual void SetWorldMatrix(dx::XMMATRIX *WorldMatrix) {}
	virtual void SetViewMatrix(dx::XMMATRIX *ViewMatrix) {}
	virtual void SetProjectionMatrix(dx::XMMATRIX *ProjectionMatrix) {}
	virtual void SetMaterial(MATERIAL Material) {}
	virtual void SetDirectionalLight(DirectionalLight* Light) {}
	virtual void SetCameraPosition(dx::XMFLOAT3* cameraPosition) {}
	virtual void SetTexture(ID3D11ShaderResourceView* texture) {}
	virtual void SetSamplerState(ID3D11SamplerState* sampler) {}

protected:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_vertexLayout;

	ID3D11Buffer* m_worldBuffer;
	ID3D11Buffer* m_viewBuffer;
	ID3D11Buffer* m_projectionBuffer;
	ID3D11Buffer* m_materialBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cameraPosBuffer;
};
