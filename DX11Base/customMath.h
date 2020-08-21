#pragma once

#include <DirectXMath.h>

namespace dx = DirectX;


// if t == 0 return a, if t == 1 return b, if t == 0.5 return midpoint of a and b
dx::XMFLOAT3 Lerp(dx::XMFLOAT3 a, dx::XMFLOAT3 b, float t);
float Lerp(float a, float b, float t);
