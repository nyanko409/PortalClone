#include "pch.h"
#include "polygoncollider.h"
#include "renderer.h"
#include "debug.h"


int PolygonCollider::m_nextId = 0;


void PolygonCollider::Init(GameObject* go, dx::XMFLOAT3 p1, dx::XMFLOAT3 p2, dx::XMFLOAT3 p3, dx::XMFLOAT3 p4, PolygonType type)
{
	m_go = go;
	m_shader = CRenderer::GetShader<LineShader>();
	m_type = type;
	m_id = m_nextId++;

	// init the normal and up vector
	dx::XMVECTOR v1 = dx::XMLoadFloat3(&(p2 - p1));
	dx::XMVECTOR v2 = dx::XMLoadFloat3(&(p4 - p1));

	dx::XMVECTOR normal = dx::XMVector3Normalize(dx::XMVector3Cross(v2, v1));
	dx::XMStoreFloat3(&m_normal, normal);
	dx::XMStoreFloat3(&m_up, dx::XMVector3Normalize(dx::XMVector3Cross(normal, v1)));

	// init the vertices
	VERTEX_3D vertices[8] = {};

	// unique vertices for polygon collision
	m_vertices[0] = p1;
	m_vertices[1] = p2;
	m_vertices[2] = p3;
	m_vertices[3] = p4;

	// vertices for drawing the collider
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
	// transform vertices, normal and up to world
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

	dx::XMVECTOR up = dx::XMLoadFloat3(&m_up);
	up = dx::XMVector3TransformNormal(up, world);
	dx::XMStoreFloat3(&m_transformedUp, up);
}

void PolygonCollider::Draw()
{
	if (!Debug::displayCollider)
		return;

	dx::XMMATRIX world = m_go->GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);

	CRenderer::DrawLine(m_shader, &m_vertexBuffer, 8);
}
