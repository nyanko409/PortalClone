#pragma once

#include "pass.h"


class GameObject
{
	friend class Scene;

public:
	GameObject() {}
	virtual ~GameObject() {}

	virtual void Awake() 
	{
		m_initialized = false;
		m_destroy = false;
		m_draw = true;
		m_enableFrustumCulling = true;
		m_disableUpdate = false;

		m_position = dx::XMFLOAT3(0, 0, 0);
		m_oldPosition = m_position;
		m_rotation = dx::XMFLOAT4(0, 0, 0, 1);
		m_scale = dx::XMFLOAT3(1, 1, 1);
		m_velocity = { 0,0,0 };

	}
	virtual void Init() { m_initialized = true; }
	virtual void Uninit() {}
	virtual void Update() {}
	virtual void Draw(Pass pass) 
	{
		// update the old position
		m_oldPosition = m_position;
	};
	virtual void Draw(const std::shared_ptr<class Shader>& shader, Pass pass)
	{
		// update the old position
		m_oldPosition = m_position;
	}

	void SetParent(GameObject* parent) { m_parent = (std::shared_ptr<GameObject>)parent; }
	void EnableFrustumCulling(bool enable) { m_enableFrustumCulling = enable; }
	void SetVelocity(dx::XMFLOAT3 velocity) { m_velocity = velocity; }

	dx::XMVECTOR GetPosition() const
	{ 
		if (auto parent = m_parent.lock())
		{
			dx::XMFLOAT4X4 worldPos;
			dx::XMStoreFloat4x4(&worldPos, GetWorldMatrix());
			return dx::XMVectorSet(worldPos._41, worldPos._42, worldPos._43, 1);
		}

		return GetLocalPosition();
	}
	dx::XMVECTOR GetLocalPosition() const { return dx::XMLoadFloat3(&m_position); }
	dx::XMVECTOR GetRotation() const { return dx::XMLoadFloat4(&m_rotation); }
	dx::XMVECTOR GetScale() const { return dx::XMLoadFloat3(&m_scale); }
	
	void SetPosition(float x, float y, float z) { m_position = dx::XMFLOAT3(x, y, z); }
	void SetPosition(dx::XMVECTOR position) { dx::XMStoreFloat3(&m_position, position); }
	void SetPosition(dx::XMFLOAT3 position) { m_position = position; }

	void SetRotation(float x, float y, float z) { dx::XMStoreFloat4(&m_rotation, dx::XMQuaternionRotationRollPitchYaw(dx::XMConvertToRadians(x), dx::XMConvertToRadians(y), dx::XMConvertToRadians(z))); }
	void SetRotation(dx::XMVECTOR rotation) { dx::XMStoreFloat4(&m_rotation, rotation); }
	void SetRotation(dx::XMFLOAT4 rotation) { m_rotation = rotation; }

	void SetScale(float x, float y, float z) { m_scale = dx::XMFLOAT3(x, y, z); }
	void SetScale(dx::XMVECTOR scale) { dx::XMStoreFloat3(&m_scale, scale); }
	void SetScale(dx::XMFLOAT3 scale) { m_scale = scale; }

	void AddPosition(dx::XMFLOAT3 translation) { m_position += translation; }
	void AddRotation(dx::XMFLOAT3 axis, float rotation) {dx::XMStoreFloat4(&m_rotation, dx::XMQuaternionMultiply(dx::XMLoadFloat4(&m_rotation), dx::XMQuaternionRotationAxis(dx::XMLoadFloat3(&axis), dx::XMConvertToRadians(rotation))));}

	void EnableUpdate(bool enable) { m_disableUpdate = !enable; }

	virtual dx::XMMATRIX GetWorldMatrix() const
	{
		dx::XMVECTOR quaternion = dx::XMLoadFloat4(&m_rotation);
		dx::XMMATRIX scale, rot, trans;

		scale = dx::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
		rot = dx::XMMatrixRotationQuaternion(quaternion);
		trans = dx::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
		
		if (auto parent = m_parent.lock())
			return scale * rot * trans * parent->GetWorldMatrix();
		else
			return scale * rot * trans;
	}

	dx::XMFLOAT3 GetForward(bool normalize = false) const
	{
		dx::XMFLOAT4X4 world;
		dx::XMStoreFloat4x4(&world, GetWorldMatrix());
		if (normalize)
		{
			dx::XMVECTOR forward = dx::XMVectorSet(world._31, world._32, world._33, 0);
			forward = dx::XMVector3Normalize(forward);
			return dx::XMFLOAT3{ dx::XMVectorGetX(forward), dx::XMVectorGetY(forward), dx::XMVectorGetZ(forward) };
		}

		return dx::XMFLOAT3(world._31, world._32, world._33);
	}

	dx::XMFLOAT3 GetRight(bool normalize = false) const
	{
		dx::XMFLOAT4X4 world;
		dx::XMStoreFloat4x4(&world, GetWorldMatrix());
		if (normalize)
		{
			dx::XMVECTOR forward = dx::XMVectorSet(world._11, world._12, world._13, 0);
			forward = dx::XMVector3Normalize(forward);
			return dx::XMFLOAT3{ dx::XMVectorGetX(forward), dx::XMVectorGetY(forward), dx::XMVectorGetZ(forward) };
		}

		return dx::XMFLOAT3(world._11, world._12, world._13);
	}

	dx::XMFLOAT3 GetUp(bool normalize = false) const
	{
		dx::XMFLOAT4X4 world;
		dx::XMStoreFloat4x4(&world, GetWorldMatrix());
		if (normalize)
		{
			dx::XMVECTOR forward = dx::XMVectorSet(world._21, world._22, world._23, 0);
			forward = dx::XMVector3Normalize(forward);
			return dx::XMFLOAT3{ dx::XMVectorGetX(forward), dx::XMVectorGetY(forward), dx::XMVectorGetZ(forward) };
		}

		return dx::XMFLOAT3(world._21, world._22, world._23);
	}

	void SetDestroy() { m_destroy = true; }
	bool Destroy()
	{
		if (m_destroy)
		{
			Uninit();
			return true;
		}

		return false;
	}

protected:
	dx::XMFLOAT3 m_position, m_oldPosition;
	dx::XMFLOAT4 m_rotation;
	dx::XMFLOAT3 m_scale;
	dx::XMFLOAT3 m_velocity;

	std::weak_ptr<GameObject> m_parent;

	bool m_destroy;
	bool m_initialized;
	bool m_draw;
	bool m_enableFrustumCulling;
	bool m_disableUpdate;
};
