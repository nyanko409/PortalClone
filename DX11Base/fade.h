#pragma once

#include "polygon.h"
#include "gameobject.h"
#include "uishader.h"


class Fade : public GameObject
{
	typedef void(*onFinished)();

public:
	void Awake() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void StartFadeOut(float fadeSpeed, onFinished function = nullptr);
	void StartFadeIn(float fadeSpeed, onFinished function = nullptr);

private:
	UIShader*					m_shader;
	ID3D11Buffer*				m_VertexBuffer;
	ID3D11ShaderResourceView*	m_Texture;

	float m_alpha;
	float m_fadeSpeed;
	onFinished m_function;
	bool m_isFadingOut;
	bool m_isFadingIn;
};
