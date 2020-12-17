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
	static dx::XMMATRIX GetProjectionMatrix(PortalType type);
	static dx::XMMATRIX GetViewMatrix(PortalType type);

	static void BindRenderTexture(PortalType type, const std::shared_ptr<RenderTexture>& renderTexture, const std::shared_ptr<RenderTexture>& renderTextureTemp);
	static void SetRecursionNum(uint32_t num) { m_recursionNum = num; }

private:
	static std::weak_ptr<Portal> m_bluePortal, m_orangePortal;
	static std::weak_ptr<RenderTexture> m_renderTexBlue, m_renderTexOrange, m_renderTexBlueTemp, m_renderTexOrangeTemp;
	static uint32_t m_recursionNum;
};
