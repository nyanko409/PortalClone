#pragma once

#include "gameobject.h"
#include "lineshader.h"


struct Line
{
	dx::XMFLOAT3 start;
	dx::XMFLOAT3 end;
};

class PolygonCollider
{
public:
	PolygonCollider() {}
	~PolygonCollider() { SAFE_RELEASE(m_vertexBuffer); }

	void Init(GameObject* go, float width, float height, float offsetX = 0, float offsetY = 0, float offsetZ = 0);
	void Draw();
	void Update();

	dx::XMFLOAT3 GetLineCollisionPoint (Line* other) const;

private:
	GameObject* m_go;
	std::shared_ptr<LineShader> m_shader;
	dx::XMFLOAT3 m_vertices[4];
	dx::XMFLOAT3 m_transformedVerts[4];
	ID3D11Buffer* m_vertexBuffer = nullptr;
};
