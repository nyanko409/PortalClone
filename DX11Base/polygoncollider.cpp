#include "pch.h"
#include "polygoncollider.h"
#include "renderer.h"


int PolygonCollider::m_nextId = 0;


void PolygonCollider::Init(GameObject* go, dx::XMFLOAT3 p1, dx::XMFLOAT3 p2, dx::XMFLOAT3 p3, dx::XMFLOAT3 p4, float normalX, float normalY, float normalZ, PolygonType type)
{
	m_go = go;
	m_shader = CRenderer::GetShader<LineShader>();
	m_type = type;
	m_id = m_nextId++;

	// init the normal vector
	m_normal = dx::XMFLOAT3(normalX, normalY, normalZ);

	// init the vertices
	VERTEX_3D vertices[8] = {};

	// unique vertices for polygon collision
	m_vertices[0] = p1;
	m_vertices[1] = p2;
	m_vertices[2] = p3;
	m_vertices[3] = p4;

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
	// transform vertices and normal to world
	dx::XMMATRIX world = m_go->GetWorldMatrix();

	for (int i = 0; i < 4; ++i)
	{
		dx::XMVECTOR vertex = dx::XMLoadFloat3(&m_vertices[i]);
		vertex = dx::XMVector3TransformCoord(vertex, world);
		dx::XMStoreFloat3(&m_transformedVerts[i], vertex);
	}

	dx::XMVECTOR normal = dx::XMLoadFloat3(&m_normal);
	normal = dx::XMVector3TransformNormal(normal, world);
	dx::XMStoreFloat3(&m_transformedNormal, normal);
}

void PolygonCollider::Draw()
{
	dx::XMMATRIX world = m_go->GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);

	CRenderer::DrawLine(m_shader, &m_vertexBuffer, 8);
}
