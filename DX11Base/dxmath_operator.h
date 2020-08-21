#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <DirectXMath.h>


namespace dx = DirectX;

dx::XMFLOAT3 operator + (const dx::XMFLOAT3& lhs, const dx::XMFLOAT3& rhs);
dx::XMFLOAT3 operator + (const dx::XMVECTOR& lhs, const dx::XMFLOAT3& rhs);
dx::XMFLOAT3 operator - (const dx::XMFLOAT3& lhs, const dx::XMFLOAT3& rhs);
dx::XMFLOAT3 operator - (const dx::XMVECTOR& lhs, const dx::XMFLOAT3& rhs);
dx::XMFLOAT3 operator * (const dx::XMFLOAT3& lhs, const float& rhs);

void operator += (dx::XMVECTOR& lhs, const dx::XMFLOAT3& rhs);
void operator += (dx::XMFLOAT3& lhs, const dx::XMVECTOR& rhs);
void operator += (dx::XMFLOAT3& lhs, const dx::XMFLOAT3& rhs);

void operator -= (dx::XMVECTOR& lhs, const dx::XMFLOAT3& rhs);
void operator -= (dx::XMFLOAT3& lhs, const dx::XMVECTOR& rhs);
void operator -= (dx::XMFLOAT3& lhs, const dx::XMFLOAT3& rhs);

