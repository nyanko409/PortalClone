#pragma once

#include "camera.h"


static class FrustumCulling
{
public:
	// called every frame in camera.cpp
	static void ConstructFrustum(float screenDepth, const dx::XMMATRIX& projectionMatrix, const dx::XMMATRIX& viewMatrix);

	// check if the given position is in the camera frustum
	static bool CheckPoint(dx::XMVECTOR position);

private:
	static dx::XMFLOAT4 m_planes[6];
};
