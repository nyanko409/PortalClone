#pragma once


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

		m_position = dx::XMFLOAT3(0, 0, 0);
		m_oldPosition = m_position;
		m_scale = dx::XMFLOAT3(1, 1, 1);

		m_rotation = dx::XMFLOAT3(0, 0, 0);
		m_quaternion = dx::XMFLOAT4(0, 0, 0, 1);
		m_prevRotation = m_rotation;
		m_diffRotation = dx::XMFLOAT3(0, 0, 0);
	}
	virtual void Init() { m_initialized = true; }
	virtual void Uninit() {}
	virtual void Update() {}
	virtual void Draw(UINT renderPass) 
	{
		// update the old position
		m_oldPosition = m_position;

		// get the diff rotation between the previous and the current frame
		m_diffRotation = m_rotation - m_prevRotation;
		m_prevRotation = m_rotation; 

		// create the quaternion that rotates by diff
		dx::XMVECTOR quaternion = dx::XMQuaternionRotationRollPitchYaw(
			dx::XMConvertToRadians(m_diffRotation.x), 
			dx::XMConvertToRadians(m_diffRotation.y), 
			dx::XMConvertToRadians(m_diffRotation.z));

		// load current quaternion from member variable
		dx::XMVECTOR curQuat = dx::XMLoadFloat4(&m_quaternion);

		// multiply the quaternions and store it back to member variable
		dx::XMVECTOR result = dx::XMQuaternionMultiply(curQuat, quaternion);
		dx::XMStoreFloat4(&m_quaternion, result);
	};
	virtual void Draw(const std::shared_ptr<class Shader>& shader, UINT renderPass)
	{
		// update the old position
		m_oldPosition = m_position;

		// get the diff rotation between the previous and the current frame
		m_diffRotation = m_rotation - m_prevRotation;
		m_prevRotation = m_rotation;

		// create the quaternion that rotates by diff
		dx::XMVECTOR quaternion = dx::XMQuaternionRotationRollPitchYaw(
			dx::XMConvertToRadians(m_diffRotation.x),
			dx::XMConvertToRadians(m_diffRotation.y),
			dx::XMConvertToRadians(m_diffRotation.z));

		// load current quaternion from member variable
		dx::XMVECTOR curQuat = dx::XMLoadFloat4(&m_quaternion);

		// multiply the quaternions and store it back to member variable
		dx::XMVECTOR result = dx::XMQuaternionMultiply(curQuat, quaternion);
		dx::XMStoreFloat4(&m_quaternion, result);
	}

	void SetParent(GameObject* parent) { m_parent = (std::shared_ptr<GameObject>)parent; }

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
	dx::XMVECTOR GetRotation() const { return dx::XMLoadFloat3(&m_rotation); }
	dx::XMVECTOR GetScale() const { return dx::XMLoadFloat3(&m_scale); }
	
	void SetPosition(float x, float y, float z) { m_position = dx::XMFLOAT3(x, y, z); }
	void SetPosition(dx::XMVECTOR position) { dx::XMStoreFloat3(&m_position, position); }
	void SetPosition(dx::XMFLOAT3 position) { m_position = position; }

	void SetRotation(float x, float y, float z) { m_rotation = dx::XMFLOAT3(x, y, z); }
	void SetRotation(dx::XMVECTOR rotation) { dx::XMStoreFloat3(&m_rotation, rotation); }
	void SetRotation(dx::XMFLOAT3 rotation) { m_rotation = rotation; }

	void SetScale(float x, float y, float z) { m_scale = dx::XMFLOAT3(x, y, z); }
	void SetScale(dx::XMVECTOR scale) { dx::XMStoreFloat3(&m_scale, scale); }
	void SetScale(dx::XMFLOAT3 scale) { m_scale = scale; }

	dx::XMMATRIX GetWorldMatrix() const
	{
		dx::XMVECTOR quaternion = dx::XMLoadFloat4(&m_quaternion);
		dx::XMMATRIX scale, rot, trans;

		scale = dx::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
		rot = dx::XMMatrixRotationQuaternion(quaternion);
		trans = dx::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

		if (auto parent = m_parent.lock())
			return scale * rot * trans * parent->GetWorldMatrix();
		else
			return scale * rot * trans;
	}

	dx::XMFLOAT3 GetForward() const
	{
		dx::XMFLOAT4X4 worldPos;
		dx::XMStoreFloat4x4(&worldPos, GetWorldMatrix());
		return dx::XMFLOAT3(worldPos._31, worldPos._32, worldPos._33);
	}

	dx::XMFLOAT3 GetRight() const
	{
		dx::XMFLOAT4X4 worldPos;
		dx::XMStoreFloat4x4(&worldPos, GetWorldMatrix());
		return dx::XMFLOAT3(worldPos._11, worldPos._12, worldPos._13);
	}

	dx::XMFLOAT3 GetUp() const
	{
		dx::XMFLOAT4X4 worldPos;
		dx::XMStoreFloat4x4(&worldPos, GetWorldMatrix());
		return dx::XMFLOAT3(worldPos._21, worldPos._22, worldPos._23);
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
	dx::XMFLOAT3 m_rotation;
	dx::XMFLOAT3 m_scale;

	dx::XMFLOAT4 m_quaternion;
	dx::XMFLOAT3 m_prevRotation;
	dx::XMFLOAT3 m_diffRotation;

	std::weak_ptr<GameObject> m_parent;

	bool m_destroy;
	bool m_initialized;
	bool m_draw;
	bool m_enableFrustumCulling;
};
