#include "pch.h"
#include "portal.h"
#include "manager.h"
#include "fpscamera.h"
#include "modelmanager.h"
#include "debug.h"


void Portal::Awake()
{
	GameObject::Awake();

	// init values
	ModelManager::GetModel(MODEL_PORTAL, m_model);

	SetPosition(0.0F, 0.0F, 0.0F);
	SetRotation(0.0F, 0.0F, 0.0F);
	SetScale(2, 2, 2);

	m_enableFrustumCulling = false;
	m_curScale = 0;
	m_finalScale = 2.0f;

	// colliders
	m_triggerCollider.Init((GameObject*)this, 1.2f, 2.5f, 2.0f, 0, 0, 0.5f);

	m_edgeColliders = std::vector<OBB*>();

	m_edgeColliders.push_back(new OBB());
	m_edgeColliders.back()->Init((GameObject*)this, 0.4f, 3, 2, 1.2f, 0, -0.99f);
	m_edgeColliders.push_back(new OBB());
	m_edgeColliders.back()->Init((GameObject*)this, 0.4f, 3, 2, -1.2f, 0, -0.99f);

	m_edgeColliders.push_back(new OBB());
	m_edgeColliders.back()->Init((GameObject*)this, 3, 0.4f, 2, 0, 2.1f, -0.99f);
	m_edgeColliders.push_back(new OBB());
	m_edgeColliders.back()->Init((GameObject*)this, 3, 0.4f, 2, 0, -2.2f, -0.99f);
}

void Portal::Update()
{
	GameObject::Update();

	// portal appear animation
	if (m_curScale < m_finalScale)
	{
		m_curScale += 0.2f;
		if (m_curScale > m_finalScale)
			m_curScale = m_finalScale;
	}
}

dx::XMMATRIX Portal::GetFakeWorldMatrix() const
{
	dx::XMVECTOR quaternion = dx::XMLoadFloat4(&m_rotation);
	dx::XMMATRIX scale, rot, trans;

	scale = dx::XMMatrixScaling(m_curScale, m_curScale, m_curScale);
	rot = dx::XMMatrixRotationQuaternion(quaternion);
	trans = dx::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	if (auto parent = m_parent.lock())
		return scale * rot * trans * parent->GetWorldMatrix();
	else
		return scale * rot * trans;
}

dx::XMMATRIX Portal::GetProjectionMatrix(bool firstIteration)
{
	if (!Debug::obliqueProjectionEnabled)
		return CManager::GetActiveScene()->GetMainCamera()->GetProjectionMatrix();

	if (auto linkedPortal = m_linkedPortal.lock())
	{
		auto cam = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera());

		dx::XMFLOAT4X4 v;
		dx::XMStoreFloat4x4(&v, (GetViewMatrix(firstIteration)));

		// Find a camera-space position on the plane (it does not matter where on the clip plane, as long as it is on it)
		dx::XMFLOAT3 position;
		dx::XMStoreFloat3(&position, linkedPortal->GetPosition());

		float Px, Py, Pz;
		Px = v._11 * position.x + v._21 * position.y + v._31 * position.z + v._41;
		Py = v._12 * position.x + v._22 * position.y + v._32 * position.z + v._42;
		Pz = v._13 * position.x + v._23 * position.y + v._33 * position.z + v._43;

		// Find the camera-space 4D reflection plane vector
		dx::XMFLOAT3 linkedForward = linkedPortal->GetForward();
		dx::XMFLOAT3 worldNormal;
		worldNormal.x = linkedForward.x;
		worldNormal.y = linkedForward.y;
		worldNormal.z = linkedForward.z;
		float Cx, Cy, Cz, Cw;
		Cx = v._11 * worldNormal.x + v._21 * worldNormal.y + v._31 * worldNormal.z;
		Cy = v._12 * worldNormal.x + v._22 * worldNormal.y + v._32 * worldNormal.z;
		Cz = v._13 * worldNormal.x + v._23 * worldNormal.y + v._33 * worldNormal.z;
		Cw = -Cx * Px - Cy * Py - Cz * Pz;

		// Modify the projection matrix so that it uses an oblique near clip plane
		dx::XMVECTOR clipPlane = dx::XMVectorSet(Cx, Cy, Cz, Cw);
		return cam->CalculateObliqueMatrix(clipPlane);
	}
}

dx::XMVECTOR Portal::GetClonedVelocity(dx::XMVECTOR velocity) const
{
	if (auto linkedPortal = m_linkedPortal.lock())
	{
		//direction vector -> in portal local -> rotate locally by y 180 -> out portal world
		velocity = dx::XMVector3TransformNormal(velocity, dx::XMMatrixInverse(nullptr, GetWorldMatrix()));
		velocity = dx::XMVector3TransformNormal(velocity, dx::XMMatrixRotationY(dx::XMConvertToRadians(180)));
		velocity = dx::XMVector3TransformNormal(velocity, linkedPortal->GetWorldMatrix());

		return velocity;
	}

	return dx::XMVECTOR{ 0,0,0 };
}

dx::XMVECTOR Portal::GetClonedPosition(dx::XMVECTOR position) const
{
	if (auto linkedPortal = m_linkedPortal.lock())
	{
		//direction vector -> in portal local -> rotate locally by y 180 -> out portal world
		position = dx::XMVector3Transform(position, dx::XMMatrixInverse(nullptr, GetWorldMatrix()));
		position = dx::XMVector3Transform(position, dx::XMMatrixRotationY(dx::XMConvertToRadians(180)));
		position = dx::XMVector3Transform(position, linkedPortal->GetWorldMatrix());

		return position;
	}

	return dx::XMVECTOR{ 0,0,0 };
}

dx::XMMATRIX Portal::GetClonedOrientationMatrix(dx::XMMATRIX matrix) const
{
	if (auto linkedPortal = m_linkedPortal.lock())
	{
		dx::XMMATRIX out = matrix;
		out *= dx::XMMatrixInverse(nullptr, GetWorldMatrix());
		out *= dx::XMMatrixRotationY(dx::XMConvertToRadians(180));
		out *= linkedPortal->GetWorldMatrix();

		return out;
	}

	return dx::XMMatrixIdentity();
}
