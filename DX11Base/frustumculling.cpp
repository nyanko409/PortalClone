#include "pch.h"
#include "frustumculling.h"
#include "camera.h"


bool FrustumCulling::CheckFrustum(dx::XMVECTOR position, dx::XMVECTOR camPosition, const dx::XMMATRIX& mView, const dx::XMMATRIX& mProjection)
{
	// get the inverse of view projection
	dx::XMMATRIX vp, invvp;
	vp = mView * mProjection;
	invvp = dx::XMMatrixInverse(nullptr, vp);

	dx::XMVECTOR vpos[4];
	dx::XMVECTOR wpos[4];

	vpos[0] = dx::XMVectorSet(-1.0F, 1.0F, 1.0F, 1.0F);
	vpos[1] = dx::XMVectorSet(1.0F, 1.0F, 1.0F, 1.0F);
	vpos[2] = dx::XMVectorSet(-1.0F, -1.0F, 1.0F, 1.0F);
	vpos[3] = dx::XMVectorSet(1.0F, -1.0F, 1.0F, 1.0F);

	wpos[0] = dx::XMVector3TransformCoord(vpos[0], invvp);
	wpos[1] = dx::XMVector3TransformCoord(vpos[1], invvp);
	wpos[2] = dx::XMVector3TransformCoord(vpos[2], invvp);
	wpos[3] = dx::XMVector3TransformCoord(vpos[3], invvp);
	
	// get the needed direction vectors
	dx::XMVECTOR v, v1, v2, n;

	v = dx::XMVectorSubtract(position, camPosition);

	// check the left frustum
	v1 = dx::XMVectorSubtract(wpos[0], camPosition);
	v2 = dx::XMVectorSubtract(wpos[2], camPosition);
	n = dx::XMVector3Cross(v1, v2);

	if (dx::XMVectorGetX(dx::XMVector3Dot(n, v)) < 0.0F)
		return false;

	// check the right frustum
	v1 = dx::XMVectorSubtract(wpos[1], camPosition);
	v2 = dx::XMVectorSubtract(wpos[3], camPosition);
	n = dx::XMVector3Cross(v1, v2);

	if (dx::XMVectorGetX(dx::XMVector3Dot(n, v)) > 0.0F)
		return false;

	// check the top frustum
	v1 = dx::XMVectorSubtract(wpos[0], camPosition);
	v2 = dx::XMVectorSubtract(wpos[1], camPosition);
	n = dx::XMVector3Cross(v1, v2);

	if (dx::XMVectorGetX(dx::XMVector3Dot(n, v)) > 0.0F)
		return false;

	// check the bottom frustum
	v1 = dx::XMVectorSubtract(wpos[2], camPosition);
	v2 = dx::XMVectorSubtract(wpos[3], camPosition);
	n = dx::XMVector3Cross(v1, v2);
	
	if (dx::XMVectorGetX(dx::XMVector3Dot(n, v)) < 0.0F)
		return false;

	return true;
}
