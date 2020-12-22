#pragma once

#include "portal.h"


class PortalManager : public GameObject
{
public:
	void Update() override;

	static void CreatePortal(PortalType type, dx::XMFLOAT3 position, dx::XMFLOAT3 lookAt, dx::XMFLOAT3 up);
	static dx::XMMATRIX GetProjectionMatrix(PortalType type);
	static dx::XMMATRIX GetViewMatrix(PortalType type);
	static std::shared_ptr<Portal> GetPortal(PortalType type) { return type == PortalType::Blue ? m_bluePortal.lock() : m_orangePortal.lock(); }

	static void BindRenderTexture(PortalType type, const std::shared_ptr<RenderTexture>& renderTexture, const std::shared_ptr<RenderTexture>& renderTextureTemp);
	static void SetRecursionNum(uint32_t num) { m_recursionNum = num; }

	static void SetPlayer(const std::shared_ptr<class Player>& player) { m_player = player; }
	static void AddPortalObject(const std::shared_ptr<GameObject>& portalObject) {};

private:
	static std::weak_ptr<Portal> m_bluePortal, m_orangePortal;
	static std::weak_ptr<RenderTexture> m_renderTexBlue, m_renderTexOrange, m_renderTexBlueTemp, m_renderTexOrangeTemp;
	static uint32_t m_recursionNum;

	static std::weak_ptr<class Player> m_player, m_clonedPlayer;
};
