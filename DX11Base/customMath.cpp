#include "custommath.h"
#include "dxmath_operator.h"


dx::XMFLOAT3 Lerp(dx::XMFLOAT3 a, dx::XMFLOAT3 b, float t)
{
	return a * (1 - t) + b * t;
}

float Lerp(float a, float b, float t)
{
	return a * (1 - t) + b * t;
}
