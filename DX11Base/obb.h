#pragma once

#include "gameobject.h"
#include "lineshader.h"


class OBB
{
public:
	OBB() {}
	~OBB() { SAFE_RELEASE(m_vertexBuffer); }

	void Init(GameObject* go, float width, float height, float depth);
	void Draw();

private:
	GameObject* m_go;
	std::shared_ptr<LineShader> m_shader;
	dx::XMFLOAT3 m_vertices[8];
	ID3D11Buffer* m_vertexBuffer;
};
