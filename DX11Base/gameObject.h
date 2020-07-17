#pragma once

#include "main.h"


class GameObject
{
public:
	GameObject() {}
	virtual ~GameObject() {}

	virtual void Init() = 0;
	virtual void Uninit() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

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

	bool m_destroy;
};
