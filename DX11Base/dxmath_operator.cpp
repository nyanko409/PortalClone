#include "dxmath_operator.h"


dx::XMFLOAT3 operator + (const dx::XMFLOAT3& lhs, const dx::XMFLOAT3& rhs)
{
	return dx::XMFLOAT3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

dx::XMFLOAT3 operator + (const dx::XMVECTOR& lhs, const dx::XMFLOAT3& rhs)
{
	dx::XMFLOAT3 fTempLhs;
	dx::XMStoreFloat3(&fTempLhs, lhs);
	return dx::XMFLOAT3( fTempLhs.x + rhs.x, fTempLhs.y + rhs.y, fTempLhs.z + rhs.z);
}

dx::XMFLOAT3 operator - (const dx::XMFLOAT3& lhs, const dx::XMFLOAT3& rhs)
{
	return dx::XMFLOAT3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

dx::XMFLOAT3 operator - (const dx::XMVECTOR& lhs, const dx::XMFLOAT3& rhs)
{
	dx::XMFLOAT3 fTempLhs;
	dx::XMStoreFloat3(&fTempLhs, lhs);
	return dx::XMFLOAT3(fTempLhs.x - rhs.x, fTempLhs.y - rhs.y, fTempLhs.z - rhs.z);
}

dx::XMFLOAT3 operator * (const dx::XMFLOAT3& lhs, const float& rhs)
{
	dx::XMVECTOR vTempLhs = dx::XMLoadFloat3(&lhs);
	vTempLhs = dx::XMVectorScale(vTempLhs, rhs);

	dx::XMFLOAT3 result;
	dx::XMStoreFloat3(&result, vTempLhs);

	return result;
}

void operator += (dx::XMVECTOR& lhs, const dx::XMFLOAT3& rhs)
{
	dx::XMVECTOR vTempRhs = dx::XMLoadFloat3(&rhs);
	lhs = dx::XMVectorAdd(lhs, vTempRhs);
}

void operator += (dx::XMFLOAT3& lhs, const dx::XMVECTOR& rhs)
{
	dx::XMVECTOR vTempLhs = dx::XMLoadFloat3(&lhs);
	vTempLhs = dx::XMVectorAdd(vTempLhs, rhs);
	dx::XMStoreFloat3(&lhs, vTempLhs);
}

void operator += (dx::XMFLOAT3& lhs, const dx::XMFLOAT3& rhs)
{
	dx::XMVECTOR vTempLhs = dx::XMLoadFloat3(&lhs);
	dx::XMVECTOR vTempRhs = dx::XMLoadFloat3(&rhs);

	vTempLhs = dx::XMVectorAdd(vTempLhs, vTempRhs);
	dx::XMStoreFloat3(&lhs, vTempLhs);
}

void operator -= (dx::XMVECTOR& lhs, const dx::XMFLOAT3& rhs)
{
	dx::XMVECTOR vTempRhs = dx::XMLoadFloat3(&rhs);
	lhs = dx::XMVectorSubtract(lhs, vTempRhs);
}

void operator -= (dx::XMFLOAT3& lhs, const dx::XMVECTOR& rhs)
{
	dx::XMVECTOR vTempLhs = dx::XMLoadFloat3(&lhs);
	vTempLhs = dx::XMVectorSubtract(vTempLhs, rhs);
	dx::XMStoreFloat3(&lhs, vTempLhs);
}

void operator -= (dx::XMFLOAT3& lhs, const dx::XMFLOAT3& rhs)
{
	dx::XMVECTOR vTempLhs = dx::XMLoadFloat3(&lhs);
	dx::XMVECTOR vTempRhs = dx::XMLoadFloat3(&rhs);

	vTempLhs = dx::XMVectorSubtract(vTempLhs, vTempRhs);
	dx::XMStoreFloat3(&lhs, vTempLhs);
}

bool operator == (const dx::XMFLOAT3& lhs, const dx::XMFLOAT3& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == lhs.z;
}
