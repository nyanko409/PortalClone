#pragma once


static class Debug
{
public:
	static int cameraNum;
	static bool obliqueProjectionEnabled;
	static bool portalClipping;
	static bool displayCollider;
	static bool pauseUpdate;

	static void Draw();
};
