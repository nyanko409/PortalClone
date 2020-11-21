#pragma once

#include "camera.h"


static class FrustumCulling
{
public:
	// called every frame in camera.cpp
	static void ConstructFrustum(float screenDepth, const dx::XMMATRIX& projectionMatrix, const dx::XMMATRIX& viewMatrix);

	// check if the given point is inside the camera frustum
	static bool CheckPoint(dx::XMVECTOR position);

	// check if the given sphere is inside the camera frustum
	static bool CheckSphere(dx::XMVECTOR position, float radius);

private:
	static dx::XMFLOAT4 m_planes[6];
};
