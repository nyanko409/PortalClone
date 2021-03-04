#include "pch.h"
#include "collision.h"
#include <limits>
#include <algorithm>


dx::XMFLOAT3 Collision::ObbObbCollision(OBB* obb1, OBB* obb2)
{
	obb1->Update();
	obb2->Update();

	dx::XMVECTOR axis1, axis2;
	float intersectLength = std::numeric_limits<float>().max();
	dx::XMFLOAT3 intersectAxis;

	// check all 15 axes for intersection
	// OBB 1 axes
	axis1 = dx::XMLoadFloat3(&(obb1->m_transformedVerts[4] - obb1->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis1 = dx::XMLoadFloat3(&(obb1->m_transformedVerts[1] - obb1->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis1 = dx::XMLoadFloat3(&(obb1->m_transformedVerts[3] - obb1->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	// OBB 2 axes
	axis1 = dx::XMLoadFloat3(&(obb2->m_transformedVerts[4] - obb2->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis1 = dx::XMLoadFloat3(&(obb2->m_transformedVerts[1] - obb2->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis1 = dx::XMLoadFloat3(&(obb2->m_transformedVerts[3] - obb2->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	// cross a0
	axis1 = dx::XMLoadFloat3(&(obb1->m_transformedVerts[4] - obb1->m_transformedVerts[0]));
	axis2 = dx::XMLoadFloat3(&(obb2->m_transformedVerts[4] - obb2->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis2 = dx::XMLoadFloat3(&(obb2->m_transformedVerts[1] - obb2->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis2 = dx::XMLoadFloat3(&(obb2->m_transformedVerts[3] - obb2->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	// cross a1
	axis1 = dx::XMLoadFloat3(&(obb1->m_transformedVerts[1] - obb1->m_transformedVerts[0]));
	axis2 = dx::XMLoadFloat3(&(obb2->m_transformedVerts[4] - obb2->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis2 = dx::XMLoadFloat3(&(obb2->m_transformedVerts[1] - obb2->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis2 = dx::XMLoadFloat3(&(obb2->m_transformedVerts[3] - obb2->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	// cross a2
	axis1 = dx::XMLoadFloat3(&(obb1->m_transformedVerts[3] - obb1->m_transformedVerts[0]));
	axis2 = dx::XMLoadFloat3(&(obb2->m_transformedVerts[4] - obb2->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis2 = dx::XMLoadFloat3(&(obb2->m_transformedVerts[1] - obb2->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis2 = dx::XMLoadFloat3(&(obb2->m_transformedVerts[3] - obb2->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb1->m_transformedVerts, obb2->m_transformedVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	// hit on all axes
	return intersectAxis * intersectLength;
}

dx::XMFLOAT3 Collision::ObbPolygonCollision(OBB* obb, PolygonCollider* polygon, float polygonWidth)
{
	obb->Update();
	polygon->Update();

	dx::XMVECTOR axis1, axis2;
	float intersectLength = std::numeric_limits<float>().max();
	dx::XMFLOAT3 intersectAxis;

	// add width to polygon
	dx::XMFLOAT3 polygonVerts[8] =
	{
		polygon->m_transformedVerts[0], polygon->m_transformedVerts[1],
		polygon->m_transformedVerts[2], polygon->m_transformedVerts[3],
		polygon->m_transformedVerts[0] - polygon->m_transformedNormal * polygonWidth, 
		polygon->m_transformedVerts[1] - polygon->m_transformedNormal * polygonWidth,
		polygon->m_transformedVerts[2] - polygon->m_transformedNormal * polygonWidth, 
		polygon->m_transformedVerts[3] - polygon->m_transformedNormal * polygonWidth
	};

	// check all axes for intersection
	// OBB axes
	axis1 = dx::XMLoadFloat3(&(obb->m_transformedVerts[4] - obb->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis1 = dx::XMLoadFloat3(&(obb->m_transformedVerts[1] - obb->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis1 = dx::XMLoadFloat3(&(obb->m_transformedVerts[3] - obb->m_transformedVerts[0]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	// polygon axes
	axis1 = dx::XMLoadFloat3(&(polygonVerts[1] - polygonVerts[0]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);
	
	axis1 = dx::XMLoadFloat3(&(polygonVerts[0] - polygonVerts[2]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis1 = dx::XMLoadFloat3(&(polygonVerts[4] - polygonVerts[0]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(axis1), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	// cross a0
	axis1 = dx::XMLoadFloat3(&(obb->m_transformedVerts[4] - obb->m_transformedVerts[0]));
	axis2 = dx::XMLoadFloat3(&(polygonVerts[4] - polygonVerts[0]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis2 = dx::XMLoadFloat3(&(polygonVerts[1] - polygonVerts[0]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis2 = dx::XMLoadFloat3(&(polygonVerts[2] - polygonVerts[0]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	// cross a1
	axis1 = dx::XMLoadFloat3(&(obb->m_transformedVerts[1] - obb->m_transformedVerts[0]));
	axis2 = dx::XMLoadFloat3(&(polygonVerts[4] - polygonVerts[0]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis2 = dx::XMLoadFloat3(&(polygonVerts[1] - polygonVerts[0]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis2 = dx::XMLoadFloat3(&(polygonVerts[2] - polygonVerts[0]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	// cross a2
	axis1 = dx::XMLoadFloat3(&(obb->m_transformedVerts[3] - obb->m_transformedVerts[0]));
	axis2 = dx::XMLoadFloat3(&(polygonVerts[4] - polygonVerts[0]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis2 = dx::XMLoadFloat3(&(polygonVerts[1] - polygonVerts[0]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);

	axis2 = dx::XMLoadFloat3(&(polygonVerts[2] - polygonVerts[0]));
	if (!IntersectsWhenProjected(obb->m_transformedVerts, polygonVerts, dx::XMVector3Normalize(dx::XMVector3Cross(axis1, axis2)), intersectLength, intersectAxis))
		return dx::XMFLOAT3(0, 0, 0);
	

	// hit on all axes
	return intersectAxis * intersectLength;
}

bool Collision::IntersectsWhenProjected(dx::XMFLOAT3 a[], dx::XMFLOAT3 b[], dx::XMVECTOR axis, float& intersectLength, dx::XMFLOAT3& intersectAxis)
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
		float aDist = dx::XMVectorGetX(dx::XMVector3Dot(aVec, axis));
		aMin = (aDist < aMin) ? aDist : aMin;
		aMax = (aDist > aMax) ? aDist : aMax;

		dx::XMVECTOR bVec = dx::XMLoadFloat3(&b[i]);
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

void Collision::AdjustCollisionOffset(PolygonCollider* polygon, dx::XMFLOAT3& hitPosition)
{
	bool xNormal = fabsf(polygon->m_transformedNormal.x) >= 0.7f;
	float yMin = std::fmin(polygon->m_transformedVerts[0].y, polygon->m_transformedVerts[2].y);
	float yMax = std::fmaxf(polygon->m_transformedVerts[0].y, polygon->m_transformedVerts[2].y);

	float xzMin, xzMax;
	if (xNormal)
	{
		xzMin = std::fmin(polygon->m_transformedVerts[0].z, polygon->m_transformedVerts[1].z);
		xzMax = std::fmaxf(polygon->m_transformedVerts[0].z, polygon->m_transformedVerts[1].z);
	}
	else
	{
		xzMin = std::fmin(polygon->m_transformedVerts[0].x, polygon->m_transformedVerts[1].x);
		xzMax = std::fmaxf(polygon->m_transformedVerts[0].x, polygon->m_transformedVerts[1].x);
	}

	dx::XMFLOAT3 xzDir = polygon->m_transformedVerts[1] - polygon->m_transformedVerts[0];
	dx::XMFLOAT3 yDir = polygon->m_transformedVerts[0] - polygon->m_transformedVerts[3];
	if (!xNormal && polygon->m_transformedNormal.z < 0)
		xzDir = xzDir * -1;
	else if (xNormal && polygon->m_transformedNormal.x > 0)
		xzDir = xzDir * -1;

	dx::XMVECTOR vecNorm = dx::XMLoadFloat3(&xzDir);
	vecNorm = dx::XMVector3Normalize(vecNorm);
	dx::XMStoreFloat3(&xzDir, vecNorm);

	vecNorm = dx::XMLoadFloat3(&yDir);
	vecNorm = dx::XMVector3Normalize(vecNorm);
	dx::XMStoreFloat3(&yDir, vecNorm);

	float portalWidth = 2.0f;
	float portalheight = 4.0f;
	float offset = 0;

	offset = hitPosition.y + portalheight - yMax;
	hitPosition = offset > 0 ? hitPosition - (yDir * offset) : hitPosition;
	offset = hitPosition.y - portalheight - yMin;
	hitPosition = offset < 0 ? hitPosition - (yDir * offset) : hitPosition;

	float pos = xNormal ? hitPosition.z : hitPosition.x;
	offset = pos + portalWidth - xzMax;
	hitPosition = offset > 0 ? hitPosition - (xzDir * offset) : hitPosition;
	offset = pos - portalWidth - xzMin;
	hitPosition = offset < 0 ? hitPosition - (xzDir * offset) : hitPosition;
}

bool Collision::LinePolygonCollision(PolygonCollider* polygon, dx::XMFLOAT3 point, dx::XMFLOAT3 direction, dx::XMFLOAT3& outCollisionPoint, dx::XMFLOAT3& outNormal, dx::XMFLOAT3& outUp)
{
	dx::XMVECTOR vecStart, vecEnd, vecNormal, vecScaledEnd;
	vecStart = dx::XMLoadFloat3(&point);
	vecEnd = dx::XMLoadFloat3(&direction);
	vecNormal = dx::XMLoadFloat3(&polygon->m_transformedNormal);
	vecScaledEnd = dx::XMVectorScale(vecEnd, 100);

	// check if there is a chance of collision (broad phase)
	if (dx::XMVectorGetX(dx::XMVector3Dot(vecNormal, vecEnd)) >= 0.0F)
		return false;

	// there might be a collision inside the plane, calculate the point (narrow phase)
	auto v1 = dx::XMLoadFloat3(&(point - polygon->m_transformedVerts[0]));
	auto v2 = dx::XMLoadFloat3(&(dx::XMVectorAdd(vecStart, vecScaledEnd) - polygon->m_transformedVerts[0]));
	float d1 = dx::XMVectorGetX(dx::XMVector3Dot(vecNormal, v1));
	float d2 = dx::XMVectorGetX(dx::XMVector3Dot(vecNormal, v2));

	// check if the line is intersecting
	if (!((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)))
	{
		return false;
	}

	d1 = fabsf(d1);
	d2 = fabsf(d2);

	float a = d1 / (d1 + d2);
	float a2 = 1 - a;

	auto v3 = dx::XMVectorAdd(dx::XMVectorScale(v1, a2), dx::XMVectorScale(v2, a));

	outCollisionPoint = v3 + polygon->m_transformedVerts[0];

	// we got the collision point, check if the point is inside the plane (simple aabb test) and offset it if nessecary
	if (fabsf(polygon->m_transformedNormal.x) >= 0.7f)
	{
		if (!((outCollisionPoint.y <= polygon->m_transformedVerts[2].y && outCollisionPoint.y >= polygon->m_transformedVerts[0].y ||
			outCollisionPoint.y >= polygon->m_transformedVerts[2].y && outCollisionPoint.y <= polygon->m_transformedVerts[0].y) &&
			((outCollisionPoint.z >= polygon->m_transformedVerts[0].z && outCollisionPoint.z <= polygon->m_transformedVerts[1].z) ||
				outCollisionPoint.z <= polygon->m_transformedVerts[0].z && outCollisionPoint.z >= polygon->m_transformedVerts[1].z)))
		{
			return false;
		}

		AdjustCollisionOffset(polygon, outCollisionPoint);
	}
	else if (fabsf(polygon->m_transformedNormal.z) >= 0.7f)
	{
		if (!((outCollisionPoint.y <= polygon->m_transformedVerts[2].y && outCollisionPoint.y >= polygon->m_transformedVerts[0].y ||
			outCollisionPoint.y >= polygon->m_transformedVerts[2].y && outCollisionPoint.y <= polygon->m_transformedVerts[0].y) &&
			((outCollisionPoint.x >= polygon->m_transformedVerts[0].x && outCollisionPoint.x <= polygon->m_transformedVerts[1].x) ||
				outCollisionPoint.x <= polygon->m_transformedVerts[0].x && outCollisionPoint.x >= polygon->m_transformedVerts[1].x)))
		{
			return false;
		}

		AdjustCollisionOffset(polygon, outCollisionPoint);
	}
	else if (fabsf(polygon->m_transformedNormal.y) >= 0.7f)
	{
		if (!((outCollisionPoint.z <= polygon->m_transformedVerts[2].z && outCollisionPoint.z >= polygon->m_transformedVerts[0].z ||
			outCollisionPoint.z >= polygon->m_transformedVerts[2].z && outCollisionPoint.z <= polygon->m_transformedVerts[0].z) &&
			((outCollisionPoint.x >= polygon->m_transformedVerts[0].x && outCollisionPoint.x <= polygon->m_transformedVerts[1].x) ||
				outCollisionPoint.x <= polygon->m_transformedVerts[0].x && outCollisionPoint.x >= polygon->m_transformedVerts[1].x)))
		{
			return false;
		}
	}

	// its inside the plane
	outNormal = polygon->m_transformedNormal;
	outUp = polygon->m_transformedUp;

	if(fabsf(outNormal.y) >= 0.99f)
	{
		vecEnd = dx::XMVectorSetY(vecEnd, 0);
		vecEnd = dx::XMVector3Normalize(vecEnd);
		dx::XMStoreFloat3(&outUp, vecEnd);
	}

	return true;
}
