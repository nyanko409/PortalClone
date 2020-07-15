#pragma once

#include "scene.h"


class CManager
{
private:
	static class Scene* m_scene;

public:
	static void Init();
	static void Uninit();
	static void Update();
	static void Draw();

	static class Scene* GetActiveScene();
};
