#pragma once

#include "gameobject.h"
#include "polygon.h"
#include "uishader.h"


class ReloadUI : public GameObject
{
public:
	void Awake() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

private:
	UIShader*					m_shader;
	ID3D11Buffer*				m_vertexBuffer;
};
