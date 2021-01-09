#include "pch.h"
#include "obbcollider.h"
#include "renderer.h"
#include <limits>
#include <algorithm>


void OBB::Init(GameObject* go, float width, float height, float depth, float offsetX, float offsetY, float offsetZ)
{
	m_go = go;
	m_shader = CRenderer::GetShader<LineShader>();

	// init the vertices
	VERTEX_3D vertices[24] = {};
	float halfW = width / 2, halfH = height / 2, halfD = depth / 2;

	// unique vertices for SAT
	m_vertices[0] = dx::XMFLOAT3(-halfW + offsetX, -halfH + offsetY, -halfD + offsetZ);
	m_vertices[1] = dx::XMFLOAT3(halfW + offsetX, -halfH + offsetY, -halfD + offsetZ);
	m_vertices[2] = dx::XMFLOAT3(halfW + offsetX, halfH + offsetY, -halfD + offsetZ);
	m_vertices[3] = dx::XMFLOAT3(-halfW + offsetX, halfH + offsetY, -halfD + offsetZ);
	m_vertices[4] = dx::XMFLOAT3(-halfW + offsetX, -halfH + offsetY, halfD + offsetZ);
	m_vertices[5] = dx::XMFLOAT3(halfW + offsetX, -halfH + offsetY, halfD + offsetZ);
	m_vertices[6] = dx::XMFLOAT3(halfW + offsetX, halfH + offsetY, halfD + offsetZ);
	m_vertices[7] = dx::XMFLOAT3(-halfW + offsetX, halfH + offsetY, halfD + offsetZ);
	
	// vertices for drawing the line
	// back
	vertices[0].Position = m_vertices[0];
	vertices[1].Position = m_vertices[1];
	vertices[2].Position = m_vertices[1];
	vertices[3].Position = m_vertices[2];
	vertices[4].Position = m_vertices[2];
	vertices[5].Position = m_vertices[3];
	vertices[6].Position = m_vertices[3];
	vertices[7].Position = m_vertices[0];
	
	// front
	vertices[8].Position = m_vertices[4];
	vertices[9].Position = m_vertices[5];
	vertices[10].Position = m_vertices[5];
	vertices[11].Position = m_vertices[6];
	vertices[12].Position = m_vertices[6];
	vertices[13].Position = m_vertices[7];
	vertices[14].Position = m_vertices[7];
	vertices[15].Position = m_vertices[4];

	// mid
	vertices[16].Position = m_vertices[0];
	vertices[17].Position = m_vertices[4];
	vertices[18].Position = m_vertices[1];
	vertices[19].Position = m_vertices[5];
	vertices[20].Position = m_vertices[3];
	vertices[21].Position = m_vertices[7];
	vertices[22].Position = m_vertices[2];
	vertices[23].Position = m_vertices[6];

	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(VERTEX_3D) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertices;

	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer);
}

void OBB::Update()
{
	// transform vertices to world
	dx::XMMATRIX world = m_go->GetWorldMatrix();

	for (int i = 0; i < 8; ++i)
	{
		dx::XMVECTOR vertex = dx::XMLoadFloat3(&m_vertices[i]);
		vertex = dx::XMVector3Transform(vertex, world);
		dx::XMStoreFloat3(&m_transformedVerts[i], vertex);
	}
}

void OBB::Draw()
{
	dx::XMMATRIX world = m_go->GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);
	
	CRenderer::DrawLine(m_shader, &m_vertexBuffer, 24);
}
