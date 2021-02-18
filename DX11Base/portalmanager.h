#pragma once

#include "portal.h"


enum class PortalTechnique { RenderToTexture, Stencil };

class PortalManager : public GameObject
{
public:
	void LateUpdate() override;

	static void SetPortalTechnique(PortalTechnique technique);
	static PortalTechnique GetPortalTechnique() { return m_technique; }

	static void CreatePortal(PortalType type, dx::XMFLOAT3 position, dx::XMFLOAT3 lookAt, dx::XMFLOAT3 up, int colId);
	static dx::XMMATRIX GetProjectionMatrix(PortalType type);
	static dx::XMMATRIX GetViewMatrix(PortalType type);
	static std::shared_ptr<Portal> GetPortal(PortalType type);

	static void BindRenderTexture(PortalType type, const std::shared_ptr<RenderTexture>& renderTexture, const std::shared_ptr<RenderTexture>& renderTextureTemp);
	static int GetRecursionNum() { return m_recursionNum; }
	static void SetRecursionNum(int num);

	static void AddPortalTraveler(const std::shared_ptr<class PortalTraveler>& traveler) { m_travelers.push_back(traveler); };

private:
	static std::weak_ptr<Portal> m_bluePortal, m_orangePortal;
	static std::weak_ptr<RenderTexture> m_renderTexBlue, m_renderTexOrange, m_renderTexBlueTemp, m_renderTexOrangeTemp;
	static int m_recursionNum;
	static PortalTechnique m_technique;

	static std::vector<std::weak_ptr<class PortalTraveler>> m_travelers;
};
