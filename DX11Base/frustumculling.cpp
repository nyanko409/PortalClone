#include "pch.h"
#include "frustumculling.h"


dx::XMFLOAT4 FrustumCulling::m_planes[6];


void FrustumCulling::ConstructFrustum(float screenDepth, const dx::XMMATRIX& projectionMatrix, const dx::XMMATRIX& viewMatrix)
{
	float zMinimum, r;
	dx::XMMATRIX matrix;
	dx::XMFLOAT4X4 fMatrix;

	// Calculate the minimum Z distance in the frustum.
	dx::XMFLOAT4X4 proj;
	dx::XMStoreFloat4x4(&proj, projectionMatrix);

	zMinimum = -proj._43 / proj._33;
	r = screenDepth / (screenDepth - zMinimum);

	proj._33 = r;
	proj._43 = -r * zMinimum;

	matrix = dx::XMLoadFloat4x4(&proj);
	
	// Create the frustum matrix from the view matrix and updated projection matrix
	matrix = viewMatrix * matrix;
	dx::XMStoreFloat4x4(&fMatrix, matrix);

	// Calculate near plane of frustum.
	m_planes[0].x = fMatrix._14 + fMatrix._13;
	m_planes[0].y = fMatrix._24 + fMatrix._23;
	m_planes[0].z = fMatrix._34 + fMatrix._33;
	m_planes[0].w = fMatrix._44 + fMatrix._43;
	

	// Calculate far plane of frustum.
	m_planes[1].x = fMatrix._14 - fMatrix._13;
	m_planes[1].y = fMatrix._24 - fMatrix._23;
	m_planes[1].z = fMatrix._34 - fMatrix._33;
	m_planes[1].w = fMatrix._44 - fMatrix._43;

	// Calculate left plane of frustum.
	m_planes[2].x = fMatrix._14 + fMatrix._11;
	m_planes[2].y = fMatrix._24 + fMatrix._21;
	m_planes[2].z = fMatrix._34 + fMatrix._31;
	m_planes[2].w = fMatrix._44 + fMatrix._41;

	// Calculate right plane of frustum.
	m_planes[3].x = fMatrix._14 - fMatrix._11;
	m_planes[3].y = fMatrix._24 - fMatrix._21;
	m_planes[3].z = fMatrix._34 - fMatrix._31;
	m_planes[3].w = fMatrix._44 - fMatrix._41;

	// Calculate top plane of frustum.
	m_planes[4].x = fMatrix._14 - fMatrix._12;
	m_planes[4].y = fMatrix._24 - fMatrix._22;
	m_planes[4].z = fMatrix._34 - fMatrix._32;
	m_planes[4].w = fMatrix._44 - fMatrix._42;

	// Calculate bottom plane of frustum.
	m_planes[5].x = fMatrix._14 + fMatrix._12;
	m_planes[5].y = fMatrix._24 + fMatrix._22;
	m_planes[5].z = fMatrix._34 + fMatrix._32;
	m_planes[5].w = fMatrix._44 + fMatrix._42;

	// normalize the planes
	for (int i = 0; i < 6; ++i)
	{
		dx::XMVECTOR temp = dx::XMLoadFloat4(&m_planes[i]);
		temp = dx::XMPlaneNormalize(temp);
		dx::XMStoreFloat4(&m_planes[i], temp);
	}
}

bool FrustumCulling::CheckPoint(dx::XMVECTOR position)
{
	// Check if the point is inside all six planes of the view frustum
	for (int i = 0; i < 6; ++i)
	{
		dx::XMVECTOR plane;
		plane = dx::XMLoadFloat4(&m_planes[i]);

		float res = dx::XMVectorGetX(dx::XMPlaneDotCoord(plane, position));
		if (res < 0.0F)
		{
			return false;
		}
	}

	return true;
}
