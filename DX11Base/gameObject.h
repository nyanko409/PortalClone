#pragma once


class GameObject
{
public:
	GameObject() {}
	virtual ~GameObject() {}

	virtual void Init() {}
	virtual void Uninit() {}
	virtual void Update() {}
	virtual void Draw() 
	{
		// get the diff rotation between the previous and the current frame
		m_diffRotation = m_rotation - m_prevRotation;
		m_prevRotation = m_rotation; 

		// create the quaternion that rotates by diff
		dx::XMVECTOR quaternionA = dx::XMQuaternionRotationRollPitchYaw(m_diffRotation.x, m_diffRotation.y, m_diffRotation.z);

		// load curent rotation from member variable
		dx::XMVECTOR curQuat = dx::XMLoadFloat4(&m_quaternion);

		// multiply the quaternions and store it back to member variable
		dx::XMVECTOR result = dx::XMQuaternionMultiply(curQuat, quaternionA);
		dx::XMStoreFloat4(&m_quaternion, result);
	};

	dx::XMVECTOR GetPosition() { return dx::XMLoadFloat3(&m_position); }
	dx::XMVECTOR GetRotation() { return dx::XMLoadFloat3(&m_rotation); }
	dx::XMVECTOR GetScale() { return dx::XMLoadFloat3(&m_scale); }
	
	void SetPosition(float x, float y, float z) { m_position = dx::XMFLOAT3(x, y, z); }
	void SetPosition(dx::XMVECTOR position) { dx::XMStoreFloat3(&m_position, position); }
	void SetPosition(dx::XMFLOAT3 position) { m_position = position; }

	void SetRotation(float x, float y, float z) { m_rotation = dx::XMFLOAT3(x, y, z); }
	void SetRotation(dx::XMVECTOR rotation) { dx::XMStoreFloat3(&m_rotation, rotation); }
	void SetRotation(dx::XMFLOAT3 rotation) { m_rotation = rotation; }

	void SetScale(float x, float y, float z) { m_scale = dx::XMFLOAT3(x, y, z); }
	void SetScale(dx::XMVECTOR scale) { dx::XMStoreFloat3(&m_scale, scale); }
	void SetScale(dx::XMFLOAT3 scale) { m_scale = scale; }

	dx::XMFLOAT3 GetForward()
	{
		dx::XMVECTOR curQuat = dx::XMLoadFloat4(&m_quaternion);
		dx::XMMATRIX matQuat = dx::XMMatrixRotationQuaternion(curQuat);
		
		dx::XMFLOAT4X4 rotationMat;
		dx::XMStoreFloat4x4(&rotationMat, matQuat);
		return dx::XMFLOAT3(rotationMat._31, rotationMat._32, rotationMat._33);
	}

	dx::XMFLOAT3 GetRight()
	{
		dx::XMVECTOR curQuat = dx::XMLoadFloat4(&m_quaternion);
		dx::XMMATRIX matQuat = dx::XMMatrixRotationQuaternion(curQuat);

		dx::XMFLOAT4X4 rotationMat;
		dx::XMStoreFloat4x4(&rotationMat, matQuat);
		return dx::XMFLOAT3(rotationMat._11, rotationMat._12, rotationMat._13);
	}

	dx::XMFLOAT3 GetUp()
	{
		dx::XMVECTOR curQuat = dx::XMLoadFloat4(&m_quaternion);
		dx::XMMATRIX matQuat = dx::XMMatrixRotationQuaternion(curQuat);

		dx::XMFLOAT4X4 rotationMat;
		dx::XMStoreFloat4x4(&rotationMat, matQuat);
		return dx::XMFLOAT3(rotationMat._21, rotationMat._22, rotationMat._23);
	}

	void SetDestroy() { m_destroy = true; }
	bool Destroy()
	{
		if (m_destroy)
		{
			Uninit();
			delete this;
			return true;
		}

		return false;
	}

protected:
	dx::XMFLOAT3 m_position;
	dx::XMFLOAT3 m_rotation;
	dx::XMFLOAT3 m_scale;

	dx::XMFLOAT4 m_quaternion;
	dx::XMFLOAT3 m_prevRotation;
	dx::XMFLOAT3 m_diffRotation;

	bool m_destroy;
};
