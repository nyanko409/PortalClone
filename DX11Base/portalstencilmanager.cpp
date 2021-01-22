#include "pch.h"
#include "portalstencilmanager.h"
#include "manager.h"
#include "portalstenciltraveler.h"
#include "collision.h"


std::weak_ptr<PortalStencil> PortalStencilManager::m_bluePortal;
std::weak_ptr<PortalStencil> PortalStencilManager::m_orangePortal;
uint32_t PortalStencilManager::m_recursionNum;

std::vector<std::weak_ptr<PortalStencilTraveler>> PortalStencilManager::m_travelers;


void PortalStencilManager::Update()
{
	// update travelers
	for (auto t : m_travelers)
	{
		if (auto traveler = t.lock())
		{
			if (auto bluePortal = m_bluePortal.lock())
			{
				if (auto orangePortal = m_orangePortal.lock())
				{
					// see if the traveler is colliding with portal
					dx::XMFLOAT3 blueCol = Collision::ObbObbCollision(bluePortal->GetTriggerCollider(), traveler->GetOBB());
					dx::XMFLOAT3 orangeCol = Collision::ObbObbCollision(orangePortal->GetTriggerCollider(), traveler->GetOBB());

					// check for collision
					if (blueCol.x != 0 || blueCol.y != 0 || blueCol.z != 0)
						traveler->SetEntrancePortal(PortalStencilType::Blue);
					else if (orangeCol.x != 0 || orangeCol.y != 0 || orangeCol.z != 0)
						traveler->SetEntrancePortal(PortalStencilType::Orange);
					else
					{
						traveler->SetEntrancePortal(PortalStencilType::None);
						continue;
					}

					// check if the traveler is behind entrance portal, if so swap the main and clone
					if (auto portal = GetPortal(traveler->GetEntrancePortal()))
					{
						dx::XMVECTOR portalForward = dx::XMLoadFloat3(&portal->GetForward());
						dx::XMVECTOR portalToCamera = dx::XMVectorSubtract(traveler->GetTravelerPosition(), portal->GetPosition());

						float dot = dx::XMVectorGetX(dx::XMVector3Dot(portalForward, portalToCamera));
						if (dot < 0.0f)
						{
							// traveler is behind the portal, swap it
							traveler->Swap();

							if (portal->m_type == PortalStencilType::Blue)
								traveler->SetEntrancePortal(orangePortal->GetType());
							else
								traveler->SetEntrancePortal(bluePortal->GetType());
						}
					}
				}
			}
		}
	}
}

void PortalStencilManager::CreatePortal(PortalStencilType type, dx::XMFLOAT3 position, dx::XMFLOAT3 lookAt, dx::XMFLOAT3 up, int colId)
{
	auto portal = CManager::GetActiveScene()->AddGameObject<PortalStencil>(1);

	portal->SetRecursionNum(m_recursionNum);
	portal->SetAttachedColliderId(colId);

	switch (type)
	{
	case PortalStencilType::Blue:
		if (auto oldPortal = m_bluePortal.lock())
			oldPortal->SetDestroy();

		portal->SetColor({ 0,0,1,1 });
		portal->SetType(PortalStencilType::Blue);
		m_bluePortal = portal;
		break;
	case PortalStencilType::Orange:
		if (auto oldPortal = m_orangePortal.lock())
			oldPortal->SetDestroy();

		portal->SetColor({ 1,0.7f,0,1 });
		portal->SetType(PortalStencilType::Orange);
		m_orangePortal = portal;
		break;
	}

	if (type == PortalStencilType::Orange)
	{
		if (auto bluePortal = m_bluePortal.lock())
		{
			bluePortal->IsOtherPortalActive(true);
			bluePortal->SetOtherPortal(portal);

			portal->IsOtherPortalActive(true);
			portal->SetOtherPortal(bluePortal);
		}
	}

	if (type == PortalStencilType::Blue)
	{
		if (auto orangePortal = m_orangePortal.lock())
		{
			orangePortal->IsOtherPortalActive(true);
			orangePortal->SetOtherPortal(portal);

			portal->IsOtherPortalActive(true);
			portal->SetOtherPortal(orangePortal);
		}
	}

	// get the orientation from forward and up vector, and set it
	dx::XMVECTOR zaxis = dx::XMVector3Normalize(dx::XMLoadFloat3(&lookAt));
	dx::XMVECTOR yaxis = dx::XMVector3Normalize(dx::XMLoadFloat3(&up));
	dx::XMVECTOR xaxis = dx::XMVector3Cross(yaxis, zaxis);

	dx::XMFLOAT3 z, x, y;
	dx::XMStoreFloat3(&z, zaxis);
	dx::XMStoreFloat3(&x, xaxis);
	dx::XMStoreFloat3(&y, yaxis);

	dx::XMFLOAT4X4 t;
	t._11 = x.x; t._12 = x.y; t._13 = x.z;
	t._21 = y.x; t._22 = y.y; t._23 = y.z;
	t._31 = z.x; t._32 = z.y; t._33 = z.z;
	t._41 = t._42 = t._43 = 0;

	dx::XMMATRIX matrix = dx::XMLoadFloat4x4(&t);
	dx::XMVECTOR outScale, outRot, outPos;
	dx::XMMatrixDecompose(&outScale, &outRot, &outPos, matrix);

	portal->SetPosition(position);
	portal->SetRotation(outRot);
}

dx::XMMATRIX PortalStencilManager::GetProjectionMatrix(PortalStencilType type)
{
	switch (type)
	{
	case PortalStencilType::Blue:
		if (auto portal = m_bluePortal.lock())
			return portal->GetProjectionMatrix();
	case PortalStencilType::Orange:
		if (auto portal = m_orangePortal.lock())
			return portal->GetProjectionMatrix();
	default:
		return dx::XMMATRIX();
	}
}

dx::XMMATRIX PortalStencilManager::GetViewMatrix(PortalStencilType type)
{
	switch (type)
	{
	case PortalStencilType::Blue:
		if (auto portal = m_bluePortal.lock())
			return portal->GetViewMatrix();
	case PortalStencilType::Orange:
		if (auto portal = m_orangePortal.lock())
			return portal->GetViewMatrix();
	default:
		return dx::XMMATRIX();
	}
}

std::shared_ptr<PortalStencil> PortalStencilManager::GetPortal(PortalStencilType type)
{
	if (type == PortalStencilType::None)
		return nullptr;

	return type == PortalStencilType::Blue ? m_bluePortal.lock() : m_orangePortal.lock();
}
