#pragma once

#include "shader.h"


class BasicLightShader : public Shader
{
public:
	void Init() override;
	void Uninit() override;
};
