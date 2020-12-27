#include "pch.h"
#include "obb.h"
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

dx::XMFLOAT3 OBB::CheckObbCollision(OBB* other)
{
	dx::XMVECTOR axis1, axis2;
	float intersectLength = std::numeric_limits<float>().max();
	dx::XMFLOAT3 intersectAxis;

	// check all 15 axes for intersection
	// OBB 1 axes
	axis1 = dx::XMLoadFloat3(&(m_transformedVerts[4] - m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0,0,0);

	axis1 = dx::XMLoadFloat3(&(m_transformedVerts[1] - m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis1 = dx::XMLoadFloat3(&(m_transformedVerts[3] - m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	// OBB 2 axes
	axis1 = dx::XMLoadFloat3(&(other->m_transformedVerts[4] - other->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis1 = dx::XMLoadFloat3(&(other->m_transformedVerts[1] - other->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis1 = dx::XMLoadFloat3(&(other->m_transformedVerts[3] - other->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	// cross a0
	axis1 = dx::XMLoadFloat3(&(m_transformedVerts[4] - m_transformedVerts[0]));
	axis2 = dx::XMLoadFloat3(&(other->m_transformedVerts[4] - other->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);
	
	axis2 = dx::XMLoadFloat3(&(other->m_transformedVerts[1] - other->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);
	
	axis2 = dx::XMLoadFloat3(&(other->m_transformedVerts[3] - other->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);
	
	// cross a1
	axis1 = dx::XMLoadFloat3(&(m_transformedVerts[1] - m_transformedVerts[0]));
	axis2 = dx::XMLoadFloat3(&(other->m_transformedVerts[4] - other->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);
	
	axis2 = dx::XMLoadFloat3(&(other->m_transformedVerts[1] - other->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);
	
	axis2 = dx::XMLoadFloat3(&(other->m_transformedVerts[3] - other->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);
	
	// cross a2
	axis1 = dx::XMLoadFloat3(&(m_transformedVerts[3] - m_transformedVerts[0]));
	axis2 = dx::XMLoadFloat3(&(other->m_transformedVerts[4] - other->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);
	
	axis2 = dx::XMLoadFloat3(&(other->m_transformedVerts[1] - other->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);
	
	axis2 = dx::XMLoadFloat3(&(other->m_transformedVerts[3] - other->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(m_transformedVerts, other->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	// hit on all axes
	return intersectAxis * intersectLength;
}

bool OBB::IntersectsWhenProjected(dx::XMFLOAT3 a[], dx::XMFLOAT3 b[], dx::XMVECTOR axis, float& intersectLength, dx::XMFLOAT3& intersectAxis)
{
	// handles the cross product = {0,0,0} case
	dx::XMFLOAT3 axisFloat;
	dx::XMStoreFloat3(&axisFloat, axis);
	if (axisFloat.x == 0 && axisFloat.y == 0 && axisFloat.z == 0)
		return true;

	float aMin = std::numeric_limits<float>().max();
	float aMax = -std::numeric_limits<float>().max();
	float bMin = std::numeric_limits<float>().max();
	float bMax = -std::numeric_limits<float>().max();

	// get the min and max values
	for (int i = 0; i < 8; ++i) 
	{
		dx::XMVECTOR aVec = dx::XMLoadFloat3(&a[i]);
		dx::XMVECTOR bVec = dx::XMLoadFloat3(&b[i]);

		float aDist = dx::XMVectorGetX(dx::XMVector3Dot(aVec, axis));
		aMin = (aDist < aMin) ? aDist : aMin;
		aMax = (aDist > aMax) ? aDist : aMax;

		float bDist = dx::XMVectorGetX(dx::XMVector3Dot(bVec, axis));
		bMin = (bDist < bMin) ? bDist : bMin;
		bMax = (bDist > bMax) ? bDist : bMax;
	}

	// one dimensional intersection test
	float longSpan = std::max(aMax, bMax) - std::min(aMin, bMin); 
	float sumSpan = aMax - aMin + bMax - bMin;

	// set the intersection length and axis for this test
	if (sumSpan - longSpan < intersectLength)
	{
		intersectLength = sumSpan - longSpan;
		intersectAxis = axisFloat;

		// reverse axis direction if OBB A is on the left side
		if (aMax < bMax)
			intersectAxis = intersectAxis * -1;
	}

	return longSpan < sumSpan;
}
