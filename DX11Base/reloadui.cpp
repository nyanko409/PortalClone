#include "pch.h"
#include "main.h"
#include "renderer.h"
#include "reloadui.h"


void ReloadUI::Awake()
{
	GameObject::Awake();

	m_shader = CRenderer::GetShader<UIShader>();

	CPolygon::CreatePlane(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 100, 50, m_vertexBuffer, true);
}

void ReloadUI::Uninit()
{
	GameObject::Uninit();

	SAFE_RELEASE(m_vertexBuffer);
}

void ReloadUI::Update()
{
	GameObject::Update();
}

void ReloadUI::Draw()
{
	GameObject::Draw();

	// set shader
	CRenderer::SetShader(m_shader);

	//���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// material
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = dx::XMFLOAT4(0.0F, 0.0F, 0.0F, 1.0F);
	m_shader->SetMaterial(material);

	//�v���~�e�B�u�g�|���W�[�ݒ�
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�|���S���`��
	CRenderer::GetDeviceContext()->Draw(4, 0);
}
