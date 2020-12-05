#include "pch.h"
#include "polygoncollider.h"
#include "renderer.h"


void PolygonCollider::Init(GameObject* go, float width, float height, float normalX, float normalY, float normalZ, float offsetX, float offsetY, float offsetZ)
{
	m_go = go;
	m_shader = CRenderer::GetShader<LineShader>();

	// init the normal vector
	m_normal = dx::XMFLOAT3(normalX, normalY, normalZ);

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

bool PolygonCollider::GetLineCollisionPoint(dx::XMFLOAT3 point, dx::XMFLOAT3 direction, dx::XMFLOAT3& collisionPoint, dx::XMFLOAT3& normal) const
{
	dx::XMVECTOR vecStart, vecEnd, vecNormal, vecScaledEnd;
	vecStart = dx::XMLoadFloat3(&point);
	vecEnd = dx::XMLoadFloat3(&direction);
	vecNormal = dx::XMLoadFloat3(&m_transformedNormal);
	vecScaledEnd = dx::XMVectorScale(vecEnd, 100);

	// check if there is a chance of collision
	if (dx::XMVectorGetX(dx::XMVector3Dot(vecNormal, vecEnd)) >= 0.0F)
	{
		return false;
	}

	// there is a collision inside the plane, calculate the point and return it
	auto v1 = dx::XMLoadFloat3(&(point - m_transformedVerts[0]));
	auto v2 = dx::XMLoadFloat3(&(dx::XMVectorAdd(vecStart, vecScaledEnd) - m_transformedVerts[0]));
	float d1 = fabsf(dx::XMVectorGetX(dx::XMVector3Dot(vecNormal, v1)));
	float d2 = fabsf(dx::XMVectorGetX(dx::XMVector3Dot(vecNormal, v2)));

	float a = d1 / (d1 + d2);
	float a2 = 1 - a;

	auto v3 = dx::XMVectorAdd(dx::XMVectorScale(v1, a2), dx::XMVectorScale(v2, a));

	auto collision = dx::XMLoadFloat3(&(m_transformedVerts[0] - point));
	collision = dx::XMVectorAdd(collision, vecStart);
	collision = dx::XMVectorAdd(collision, v3);

	dx::XMStoreFloat3(&collisionPoint, collision);
	normal = m_transformedNormal;

	return true;
}
