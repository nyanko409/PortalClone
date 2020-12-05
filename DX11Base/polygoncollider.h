#pragma once

#include "gameobject.h"
#include "lineshader.h"


class PolygonCollider
{
public:
	PolygonCollider() {}
	~PolygonCollider() { SAFE_RELEASE(m_vertexBuffer); }

	void Init(GameObject* go, float width, float height, float normalX, float normalY, float normalZ, float offsetX = 0, float offsetY = 0, float offsetZ = 0);
	void Draw();
	void Update();

	bool GetLineCollisionPoint(dx::XMFLOAT3 point, dx::XMFLOAT3 direction, dx::XMFLOAT3& collisionPoint, dx::XMFLOAT3& normal) const;

private:
	GameObject* m_go;
	std::shared_ptr<LineShader> m_shader;
	dx::XMFLOAT3 m_vertices[4];
	dx::XMFLOAT3 m_normal;
	dx::XMFLOAT3 m_transformedVerts[4];
	dx::XMFLOAT3 m_transformedNormal;
	ID3D11Buffer* m_vertexBuffer = nullptr;
};
