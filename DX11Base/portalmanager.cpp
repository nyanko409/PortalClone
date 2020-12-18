#include "pch.h"
#include "portalmanager.h"
#include "manager.h"


std::weak_ptr<Portal> PortalManager::m_bluePortal;
std::weak_ptr<Portal> PortalManager::m_orangePortal;
std::weak_ptr<RenderTexture> PortalManager::m_renderTexBlue;
std::weak_ptr<RenderTexture> PortalManager::m_renderTexOrange;
std::weak_ptr<RenderTexture> PortalManager::m_renderTexBlueTemp;
std::weak_ptr<RenderTexture> PortalManager::m_renderTexOrangeTemp;
uint32_t PortalManager::m_recursionNum;


void PortalManager::CreatePortal(PortalType type, dx::XMFLOAT3 position, dx::XMFLOAT3 lookAt, dx::XMFLOAT3 up)
{
	auto portal = CManager::GetActiveScene()->AddGameObject<Portal>(1);
	portal->SetPosition(position);
	portal->SetLookAt(lookAt);
	portal->SetUp(up);
	portal->SetRecursionNum(m_recursionNum);
	
	switch (type)
	{
	case PortalType::Blue:
		if (auto oldPortal = m_bluePortal.lock())
			oldPortal->SetDestroy();

		if (auto renderTex = m_renderTexBlue.lock())
			portal->SetRenderTexture(renderTex);

		if (auto tempTex = m_renderTexBlueTemp.lock())
			portal->SetTempRenderTexture(tempTex);

		portal->SetColor({0,0,1,1});
		portal->SetType(PortalType::Blue);
		m_bluePortal = portal;
		break;
	case PortalType::Orange:
		if (auto oldPortal = m_orangePortal.lock())
			oldPortal->SetDestroy();

		if (auto renderTex = m_renderTexOrange.lock())
			portal->SetRenderTexture(renderTex);

		if (auto tempTex = m_renderTexOrangeTemp.lock())
			portal->SetTempRenderTexture(tempTex);

		portal->SetColor({ 1,0.7f,0,1 });
		portal->SetType(PortalType::Orange);
		m_orangePortal = portal;
		break;
	}

	if (type == PortalType::Orange)
	{
		if (auto bluePortal = m_bluePortal.lock())
		{
			bluePortal->IsOtherPortalActive(true);
			bluePortal->SetOtherPortal(portal);

			portal->IsOtherPortalActive(true);
			portal->SetOtherPortal(bluePortal);
		}
	}
	
	if (type == PortalType::Blue)
	{
		if (auto orangePortal = m_orangePortal.lock())
		{
			orangePortal->IsOtherPortalActive(true);
			orangePortal->SetOtherPortal(portal);

			portal->IsOtherPortalActive(true);
			portal->SetOtherPortal(orangePortal);
		}
	}
}

dx::XMMATRIX PortalManager::GetProjectionMatrix(PortalType type)
{
	switch (type)
	{
	case PortalType::Blue:
		if (auto portal = m_bluePortal.lock())
			return portal->GetProjectionMatrix();
	case PortalType::Orange:
		if (auto portal = m_orangePortal.lock())
			return portal->GetProjectionMatrix();
	default:
		return dx::XMMATRIX();
	}
}

dx::XMMATRIX PortalManager::GetViewMatrix(PortalType type)
{
	switch (type)
	{
	case PortalType::Blue:
		if (auto portal = m_bluePortal.lock())
			return portal->GetViewMatrix();
	case PortalType::Orange:
		if (auto portal = m_orangePortal.lock())
			return portal->GetViewMatrix();
	default:
		return dx::XMMATRIX();
	}
}

void PortalManager::BindRenderTexture(PortalType type, const std::shared_ptr<RenderTexture>& renderTexture, const std::shared_ptr<RenderTexture>& renderTextureTemp)
{
	switch (type)
	{
	case PortalType::Blue:
		m_renderTexBlue = renderTexture;
		m_renderTexBlueTemp = renderTextureTemp;
	case PortalType::Orange:
		m_renderTexOrange = renderTexture;
		m_renderTexOrangeTemp = renderTextureTemp;
	}
}
