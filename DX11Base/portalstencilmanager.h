#pragma once

#include "portalstencil.h"


class PortalStencilManager : public GameObject
{
public:
	void Update() override;

	static void CreatePortal(PortalStencilType type, dx::XMFLOAT3 position, dx::XMFLOAT3 lookAt, dx::XMFLOAT3 up, int colId);
	static dx::XMMATRIX GetProjectionMatrix(PortalStencilType type);
	static dx::XMMATRIX GetViewMatrix(PortalStencilType type);
	static std::shared_ptr<PortalStencil> GetPortal(PortalStencilType type);

	static void AddPortalTraveler(const std::shared_ptr<class PortalStencilTraveler>& traveler) { m_travelers.push_back(traveler); };

private:
	static std::weak_ptr<PortalStencil> m_bluePortal, m_orangePortal;

	static std::vector<std::weak_ptr<class PortalStencilTraveler>> m_travelers;
};
