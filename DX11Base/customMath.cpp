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

float Sign(float a)
{
	if (a > 0.0f) return 1.0f;
	if (a < 0.0f) return -1.0f;
	return 0.0f;
}

float Clamp(float min, float max, float value)
{

	if (value < min) return min;
	if (value > max) return max;
	return value;
}
