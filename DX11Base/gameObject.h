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

	D3DXVECTOR3 GetPosition() { return m_position; }
	D3DXVECTOR3 GetRotation() { return m_rotation; }
	D3DXVECTOR3 GetScale() { return m_scale; }

	void SetPosition(D3DXVECTOR3 position) { m_position = position; }
	void SetRotation(D3DXVECTOR3 rotation) { m_rotation = rotation; }
	void SetScale(D3DXVECTOR3 scale) { m_scale = scale; }

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
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_rotation;
	D3DXVECTOR3 m_scale;

	bool m_destroy;
};
