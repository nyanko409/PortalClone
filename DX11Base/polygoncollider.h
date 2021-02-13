#pragma once

#include "gameobject.h"
#include "lineshader.h"


enum  PolygonType
{
	Ceiling, Floor, Wall
};

class PolygonCollider
{
	friend class Collision;

public:
	PolygonCollider() {}
	~PolygonCollider() { SAFE_RELEASE(m_vertexBuffer); }

	// order of p1-p4 is clockwise from the view of the normal vector
	void Init(GameObject* go, dx::XMFLOAT3 p1, dx::XMFLOAT3 p2, dx::XMFLOAT3 p3, dx::XMFLOAT3 p4, PolygonType type);
	void Draw();
	void Update();

	PolygonType GetType() const { return m_type; }
	int GetId() const { return m_id; }

private:
	static int m_nextId;

	GameObject* m_go;
	std::shared_ptr<LineShader> m_shader;
	dx::XMFLOAT3 m_vertices[4];
	dx::XMFLOAT3 m_normal;
	dx::XMFLOAT3 m_transformedVerts[4];
	dx::XMFLOAT3 m_transformedNormal;
	ID3D11Buffer* m_vertexBuffer = nullptr;
	PolygonType m_type;
	int m_id;
};
