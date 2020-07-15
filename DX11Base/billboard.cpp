#include "main.h"
#include "renderer.h"
#include "billboard.h"
#include "camera.h"
#include "manager.h"


void Billboard::Init()
{
	VERTEX_3D vertex[4];

	vertex[0].Position = D3DXVECTOR3(-1.0F, 1.0F, 0);
	vertex[0].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);

	vertex[1].Position = D3DXVECTOR3(1.0F, 1.0F, 0);
	vertex[1].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

	vertex[2].Position = D3DXVECTOR3(-1.0F, -1.0F, 0);
	vertex[2].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = D3DXVECTOR2(0.0f, 1.0f);

	vertex[3].Position = D3DXVECTOR3(1.0F, -1.0F, 0);
	vertex[3].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

	//���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer);

	//�e�N�X�`���ǂݍ���
	D3DX11CreateShaderResourceViewFromFile(CRenderer::GetDevice(),
		"asset/texture/explosion.png",
		NULL,
		NULL,
		&m_texture,
		NULL);

	assert(m_texture);

	m_position = D3DXVECTOR3(0.0F, 0.0F, 0.0F);
	m_rotation = D3DXVECTOR3(0.0F, 0.0F, 0.0F);
	m_scale = D3DXVECTOR3(1.0F, 1.0F, 1.0F);
}

void Billboard::Uninit()
{
	m_vertexBuffer->Release();
	m_texture->Release();
}

void Billboard::Update()
{
	m_count++;
	m_position.x += 0.1F;

	if (m_count > 16)
	{
		SetDestroy();
		return;
	}
}

void Billboard::Draw()
{
	float x = m_count % 4 * (1.0F / 4);
	float y = m_count / 4 * (1.0F / 4);

	D3D11_MAPPED_SUBRESOURCE msr;
	CRenderer::GetDeviceContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	vertex[0].Position = D3DXVECTOR3(-1.0F, 1.0F, 0);
	vertex[0].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = D3DXVECTOR2(x, y);

	vertex[1].Position = D3DXVECTOR3(1.0F, 1.0F, 0);
	vertex[1].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = D3DXVECTOR2(x + (1.0F / 4), y);

	vertex[2].Position = D3DXVECTOR3(-1.0F, -1.0F, 0);
	vertex[2].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = D3DXVECTOR2(x, y + (1.0F / 4));

	vertex[3].Position = D3DXVECTOR3(1.0F, -1.0F, 0);
	vertex[3].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = D3DXVECTOR2(x + (1.0F / 4), y + (1.0F / 4));

	CRenderer::GetDeviceContext()->Unmap(m_vertexBuffer, 0);

	auto camera = CManager::GetActiveScene()->GetGameObjects<FPSCamera>(0);
	D3DXMATRIX view = camera.front()->GetViewMatrix();
	D3DXMATRIX invView;

	// get the inverse view matrix
	//D3DXMatrixInverse(&invView, NULL, &view);
	//invView._41 = 0.0F;
	//invView._42 = 0.0F;
	//invView._43 = 0.0F;
	
	// transpose instead of inverse (faster)
	D3DXMatrixTranspose(&invView, &view);
	invView._14 = 0.0F;
	invView._24 = 0.0F;
	invView._34 = 0.0F;
	
	//�}�g���b�N�X�ݒ�
	D3DXMATRIX scale, trans;
	D3DXMatrixScaling(&scale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixTranslation(&trans, m_position.x, m_position.y, m_position.z);
	
	CRenderer::SetWorldMatrix(&(scale * invView * trans));
	
	//���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	
	// set material
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
	CRenderer::SetMaterial(material);
	
	//�e�N�X�`���ݒ�
	CRenderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_texture);
	
	//�v���~�e�B�u�g�|���W�[�ݒ�
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	//�|���S���`��
	CRenderer::GetDeviceContext()->Draw(4, 0);
}
