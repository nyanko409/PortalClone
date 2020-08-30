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

	bool IsGaugeFilled() { return m_gaugeWidth >= m_width; }
	void ResetGauge() { m_gaugeWidth = 0; }

private:
	UIShader*					m_shader;
	ID3D11Buffer*				m_vertexBuffer;
	ID3D11Buffer*				m_vertexBufferGauge;
	float m_width, m_height;
	float m_posX, m_posY;
	float m_gaugeWidth;
	float m_fillSpeed;

	void FillGauge();
};
