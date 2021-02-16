#pragma once

#include "gameobject.h"
#include "lineshader.h"


class PolygonCollider
{
	friend class Collision;

public:
	PolygonCollider() {}
	~PolygonCollider() { SAFE_RELEASE(m_vertexBuffer); }

	// order of p1-p4 is clockwise from the view of the normal vector
	void Init(GameObject* go, dx::XMFLOAT3 p1, dx::XMFLOAT3 p2, dx::XMFLOAT3 p3, dx::XMFLOAT3 p4);
	void Draw();
	void Update();

	int GetId() const { return m_id; }

private:
	static int m_nextId;

	GameObject* m_go;
	std::shared_ptr<LineShader> m_shader;
	dx::XMFLOAT3 m_vertices[4];
	dx::XMFLOAT3 m_normal, m_up;
	dx::XMFLOAT3 m_transformedVerts[4];
	dx::XMFLOAT3 m_transformedNormal, m_transformedUp;
	ID3D11Buffer* m_vertexBuffer = nullptr;
	int m_id;
};
