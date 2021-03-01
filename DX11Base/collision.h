#pragma once

#include "polygoncollider.h"
#include "obbcollider.h"


static class Collision
{
public:
	static dx::XMFLOAT3 ObbObbCollision(OBB* obb1, OBB* obb2);
	static dx::XMFLOAT3 ObbPolygonCollision(OBB* obb, PolygonCollider* polygon);

	static bool LinePolygonCollision(PolygonCollider* polygon, dx::XMFLOAT3 point, dx::XMFLOAT3 direction, dx::XMFLOAT3& outCollisionPoint, dx::XMFLOAT3& outNormal, dx::XMFLOAT3& outUp);

private:
	// used for obb collision
	static bool IntersectsWhenProjected(dx::XMFLOAT3 a[], dx::XMFLOAT3 b[], dx::XMVECTOR axis, float& intersectLength, dx::XMFLOAT3& intersectAxis);

	// used for line polygon collision
	static void AdjustCollisionOffset(PolygonCollider* polygon, dx::XMFLOAT3& hitPosition);
};
