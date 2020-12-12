#include "pch.h"
#include "portalmanager.h"
#include "manager.h"


std::weak_ptr<Portal> PortalManager::m_bluePortal;
std::weak_ptr<Portal> PortalManager::m_orangePortal;


void PortalManager::CreatePortal(PortalType type, dx::XMFLOAT3 position, dx::XMFLOAT3 lookAt, dx::XMFLOAT3 up)
{
	auto portal = CManager::GetActiveScene()->AddGameObject<Portal>(1);
	portal->SetPosition(position);
	portal->SetLookAt(lookAt);
	portal->SetUp(up);
	
	switch (type)
	{
	case PortalType::Blue:
		if (auto oldPortal = m_bluePortal.lock())
			oldPortal->SetDestroy();

		portal->SetColor({0,0,1,1});
		m_bluePortal = portal;
		break;
	case PortalType::Orange:
		if (auto portal = m_orangePortal.lock())
			portal->SetDestroy();

		portal->SetColor({ 1,0.7f,0,1 });
		m_orangePortal = portal;
		break;
	}
}

dx::XMMATRIX PortalManager::GetVPMatrix(PortalType type)
{
	switch (type)
	{
	case PortalType::Blue:
		if (auto portal = m_bluePortal.lock())
			return portal->GetViewMatrix() * portal->GetProjectionMatrix();
	case PortalType::Orange:
		if (auto portal = m_orangePortal.lock())
			return portal->GetViewMatrix() * portal->GetProjectionMatrix();
	default:
		return dx::XMMATRIX();
	}
}
