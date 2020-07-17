#pragma once

#include "main.h"


void operator += (dx::XMVECTOR& lhs, const dx::XMFLOAT3& rhs);

void operator += (dx::XMFLOAT3& lhs, const dx::XMVECTOR& rhs);

void operator += (dx::XMFLOAT3& lhs, const dx::XMFLOAT3& rhs);

void operator -= (dx::XMVECTOR& lhs, const dx::XMFLOAT3& rhs);

void operator -= (dx::XMFLOAT3& lhs, const dx::XMVECTOR& rhs);

dx::XMFLOAT3 operator * (const dx::XMFLOAT3& rhs, const float& lhs);
