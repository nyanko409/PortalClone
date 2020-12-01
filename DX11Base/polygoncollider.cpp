#include "pch.h"
#include "polygoncollider.h"
#include "renderer.h"


void PolygonCollider::Init(GameObject* go, float width, float height, float offsetX, float offsetY, float offsetZ)
{
	m_go = go;
	m_shader = CRenderer::GetShader<LineShader>();

	// init the vertices
	VERTEX_3D vertices[8] = {};
	float halfW = width / 2, halfH = height / 2;

	// unique vertices for polygon collision
	m_vertices[0] = dx::XMFLOAT3(-halfW + offsetX, -halfH + offsetY, offsetZ);
	m_vertices[1] = dx::XMFLOAT3(halfW + offsetX, -halfH + offsetY, offsetZ);
	m_vertices[2] = dx::XMFLOAT3(halfW + offsetX, halfH + offsetY, offsetZ);
	m_vertices[3] = dx::XMFLOAT3(-halfW + offsetX, halfH + offsetY, offsetZ);

	// vertices for drawing the line
	vertices[0].Position = m_vertices[0];
	vertices[1].Position = m_vertices[1];
	vertices[2].Position = m_vertices[1];
	vertices[3].Position = m_vertices[2];
	vertices[4].Position = m_vertices[2];
	vertices[5].Position = m_vertices[3];
	vertices[6].Position = m_vertices[3];
	vertices[7].Position = m_vertices[0];

	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(VERTEX_3D) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertices;

	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer);
}

void PolygonCollider::Update()
{
	// transform vertices to world
	dx::XMMATRIX world = m_go->GetWorldMatrix();

	for (int i = 0; i < 4; ++i)
	{
		dx::XMVECTOR vertex = dx::XMLoadFloat3(&m_vertices[i]);
		vertex = dx::XMVector3TransformCoord(vertex, world);
		dx::XMStoreFloat3(&m_transformedVerts[i], vertex);
	}
}

void PolygonCollider::Draw()
{
	dx::XMMATRIX world = m_go->GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);

	CRenderer::DrawLine(m_shader, &m_vertexBuffer, 8);
}

dx::XMFLOAT3 PolygonCollider::GetLineCollisionPoint(Line* other)
{
	return dx::XMFLOAT3();
}
