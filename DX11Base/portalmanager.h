#pragma once

#include "portal.h"


enum class PortalType
{
	Blue, Orange
};

static class PortalManager
{
public:
	static void CreatePortal(PortalType type, dx::XMFLOAT3 position, dx::XMFLOAT3 lookAt, dx::XMFLOAT3 up);
	static dx::XMMATRIX GetVPMatrix(PortalType type);

private:
	static std::weak_ptr<Portal> m_bluePortal, m_orangePortal;
};