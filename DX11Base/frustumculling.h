#pragma once

#include "camera.h"


static class FrustumCulling
{
public:
	static bool CheckFrustum(dx::XMVECTOR position, dx::XMVECTOR camPosition, const dx::XMMATRIX& mView, const dx::XMMATRIX& mProjection);
};
