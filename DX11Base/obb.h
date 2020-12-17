#pragma once

#include "gameobject.h"
#include "lineshader.h"


class OBB
{
public:
	OBB() {}
	~OBB() { SAFE_RELEASE(m_vertexBuffer); }

	void Init(GameObject* go, float width, float height, float depth, float offsetX = 0, float offsetY = 0, float offsetZ = 0);
	void Draw();
	void Update();

	dx::XMFLOAT3 CheckObbCollision(OBB* other);

private:
	GameObject* m_go;
	std::shared_ptr<LineShader> m_shader;
	dx::XMFLOAT3 m_vertices[8];
	dx::XMFLOAT3 m_transformedVerts[8];
	ID3D11Buffer* m_vertexBuffer = nullptr;

	bool IntersectsWhenProjected(dx::XMFLOAT3 a[], dx::XMFLOAT3 b[], dx::XMVECTOR axis, float& intersectLength, dx::XMFLOAT3& intersectAxis);
};
