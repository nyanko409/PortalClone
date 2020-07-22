#include "dxmath_operator.h"


dx::XMFLOAT3 operator + (dx::XMFLOAT3& lhs, const dx::XMFLOAT3& rhs)
{
	return dx::XMFLOAT3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

dx::XMFLOAT3 operator - (dx::XMFLOAT3& lhs, const dx::XMFLOAT3& rhs)
{
	return dx::XMFLOAT3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
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

dx::XMFLOAT3 operator * (const dx::XMFLOAT3& rhs, const float& lhs)
{
	dx::XMVECTOR vTempRhs = dx::XMLoadFloat3(&rhs);
	vTempRhs = dx::XMVectorScale(vTempRhs, lhs);

	dx::XMFLOAT3 result;
	dx::XMStoreFloat3(&result, vTempRhs);

	return result;
}
