#include "pch.h"
#include "portalmanager.h"
#include "manager.h"
#include "player.h"
#include "fpscamera.h"
#include "collision.h"


std::weak_ptr<Portal> PortalManager::m_bluePortal;
std::weak_ptr<Portal> PortalManager::m_orangePortal;
std::weak_ptr<RenderTexture> PortalManager::m_renderTexBlue;
std::weak_ptr<RenderTexture> PortalManager::m_renderTexOrange;
std::weak_ptr<RenderTexture> PortalManager::m_renderTexBlueTemp;
std::weak_ptr<RenderTexture> PortalManager::m_renderTexOrangeTemp;
uint32_t PortalManager::m_recursionNum;

std::weak_ptr<class Player> PortalManager::m_player;
std::weak_ptr<class Player> PortalManager::m_clonedPlayer;


void PortalManager::Update()
{
	// update player clone
	if (auto player = m_player.lock())
	{
		// if player clone is set, check if the player is still near portal, if not delete the clone
		if (m_clonedPlayer.lock())
		{
			if (auto bluePortal = m_bluePortal.lock())
			{
				if (auto orangePortal = m_orangePortal.lock())
				{
					dx::XMFLOAT3 blueCol = Collision::ObbObbCollision(bluePortal->GetObb(), player->GetObb());
					dx::XMFLOAT3 orangeCol = Collision::ObbObbCollision(orangePortal->GetObb(), player->GetObb());

					// if not colliding anymore, delete the clone
					if (blueCol.x == 0 && blueCol.y == 0 && blueCol.z == 0 && 
						orangeCol.x == 0 && orangeCol.y == 0 && orangeCol.z == 0)
					{
						player->UnsetEntrancePortal();
						m_clonedPlayer.reset();
					}

					// check if the player is behind entrance portal, if so swap the main and clone and retarget the main camera
					if (auto portal = player->GetEntrancePortal().lock())
					{
						//if (player->virtualUp.x <= 0.1f && player->virtualUp.y >= 0.9f && player->virtualUp.z <= 0.1f)
						{
							dx::XMVECTOR portalForward = dx::XMLoadFloat3(&portal->m_lookAt);
							dx::XMVECTOR portalToCamera = dx::XMVectorSubtract(CManager::GetActiveScene()->GetMainCamera()->GetPosition(), portal->GetPosition());

							float dot = dx::XMVectorGetX(dx::XMVector3Dot(portalForward, portalToCamera));
							if (dot < 0.0f)
							{
								// player is behind the portal, swap it
								player->Swap();

								if (portal->m_type == PortalType::Blue)
									player->SetEntrancePortal(orangePortal);
								else
									player->SetEntrancePortal(bluePortal);
							}
						}
					}
				}
			}
		}
		// if player clone is not set, check for collision and set the clone
		else
		{
			if (auto bluePortal = m_bluePortal.lock())
			{
				if (auto orangePortal = m_orangePortal.lock())
				{
					dx::XMFLOAT3 blueCol = Collision::ObbObbCollision(bluePortal->GetObb(), player->GetObb());
					dx::XMFLOAT3 orangeCol = Collision::ObbObbCollision(orangePortal->GetObb(), player->GetObb());

					// if hit, set the clone
					if (blueCol.x != 0 || blueCol.y != 0 || blueCol.z != 0)
					{
						player->SetEntrancePortal(bluePortal);
						m_clonedPlayer = player;
					}
					else if (orangeCol.x != 0 || orangeCol.y != 0 || orangeCol.z != 0)
					{
						player->SetEntrancePortal(orangePortal);
						m_clonedPlayer = player;
					}
				}
			}
		}
	}
}

void PortalManager::CreatePortal(PortalType type, dx::XMFLOAT3 position, dx::XMFLOAT3 lookAt, dx::XMFLOAT3 up, int colId)
{
	auto portal = CManager::GetActiveScene()->AddGameObject<Portal>(1);
	portal->SetPosition(position);
	portal->SetLookAt(lookAt);
	portal->SetUp(up);
	portal->SetRecursionNum(m_recursionNum);
	portal->SetAttachedColliderId(colId);
	
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
