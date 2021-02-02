#include "pch.h"
#include "portalmanager.h"
#include "manager.h"
#include "portaltraveler.h"
#include "fpscamera.h"
#include "collision.h"
#include "portalrendertexture.h"
#include "portalstencil.h"
#include "depthfromlightshader.h"
#include "main.h"


std::weak_ptr<Portal> PortalManager::m_bluePortal;
std::weak_ptr<Portal> PortalManager::m_orangePortal;
std::weak_ptr<RenderTexture> PortalManager::m_renderTexBlue;
std::weak_ptr<RenderTexture> PortalManager::m_renderTexBlueTemp;
std::weak_ptr<RenderTexture> PortalManager::m_renderTexOrange;
std::weak_ptr<RenderTexture> PortalManager::m_renderTexOrangeTemp;
PortalTechnique PortalManager::m_technique = PortalTechnique::Stencil;
int PortalManager::m_recursionNum = 1;

std::vector<std::weak_ptr<PortalTraveler>> PortalManager::m_travelers;


void PortalManager::Update()
{
	// update travelers
	for(auto t : m_travelers)
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
						traveler->SetEntrancePortal(PortalType::Blue);
					else if (orangeCol.x != 0 || orangeCol.y != 0 || orangeCol.z != 0)
						traveler->SetEntrancePortal(PortalType::Orange);
					else
					{
						traveler->SetEntrancePortal(PortalType::None);
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

							if (portal->GetType() == PortalType::Blue)
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

void PortalManager::CreatePortal(PortalType type, dx::XMFLOAT3 position, dx::XMFLOAT3 lookAt, dx::XMFLOAT3 up, int colId)
{
	std::shared_ptr<Portal> portal = nullptr;

	if (m_technique == PortalTechnique::RenderToTexture)
	{
		portal = CManager::GetActiveScene()->AddGameObject<PortalRenderTexture>(1);
		portal->SetRecursionNum(m_recursionNum);
	}
	else
		portal = CManager::GetActiveScene()->AddGameObject<PortalStencil>(1);

	portal->SetAttachedColliderId(colId);
	
	switch (type)
	{
	case PortalType::Blue:
		if (auto oldPortal = m_bluePortal.lock())
			oldPortal->SetDestroy();

		if (m_technique == PortalTechnique::RenderToTexture)
		{
			if (auto renderTex = m_renderTexBlue.lock())
				portal->SetRenderTexture(renderTex);

			if (auto tempTex = m_renderTexBlueTemp.lock())
				portal->SetTempRenderTexture(tempTex);
		}

		portal->SetColor({0,0,1,1});
		portal->SetType(PortalType::Blue);
		m_bluePortal = portal;

		if (auto orangePortal = m_orangePortal.lock())
		{
			orangePortal->SetOtherPortal(portal);
			portal->SetOtherPortal(orangePortal);
		}

		break;

	case PortalType::Orange:
		if (auto oldPortal = m_orangePortal.lock())
			oldPortal->SetDestroy();

		if (m_technique == PortalTechnique::RenderToTexture)
		{
			if (auto renderTex = m_renderTexOrange.lock())
				portal->SetRenderTexture(renderTex);

			if (auto tempTex = m_renderTexOrangeTemp.lock())
				portal->SetTempRenderTexture(tempTex);
		}

		portal->SetColor({ 1,0.7f,0,1 });
		portal->SetType(PortalType::Orange);
		m_orangePortal = portal;

		if (auto bluePortal = m_bluePortal.lock())
		{
			bluePortal->SetOtherPortal(portal);
			portal->SetOtherPortal(bluePortal);
		}

		break;
	}

	// get the orientation from forward and up vector and set it
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

std::shared_ptr<Portal> PortalManager::GetPortal(PortalType type)
{
	if (type == PortalType::None)
		return nullptr;

	return type == PortalType::Blue ? m_bluePortal.lock() : m_orangePortal.lock();
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

void PortalManager::SetRecursionNum(int num)
{
	if (num < 0)
		return;

	m_recursionNum = num; 
	SetPortalTechnique(m_technique);
}

void PortalManager::SetPortalTechnique(PortalTechnique technique)
{
	CManager::ClearRenderPasses();
	CRenderer::UnbindRenderTargetViews();
	if (auto portal = m_bluePortal.lock())
	{
		portal->SetDestroy();
		m_bluePortal.reset();
	}
	if (auto portal = m_orangePortal.lock())
	{
		portal->SetDestroy();
		m_orangePortal.reset();
	}

	m_technique = technique;
	
	// setup render passes for rendering with render texture
	if (m_technique == PortalTechnique::RenderToTexture)
	{
		// create lightmap
		auto lightDepthTexture = std::make_shared<RenderTexture>(2, 2048, 2048, true);
		CRenderer::BindRenderTargetView(lightDepthTexture);

		// write to lightmap in the first pass
		RenderPass renderPass = {};
		renderPass.targetOutput = { lightDepthTexture->GetRenderTargetViewID() };
		renderPass.clearRTV = true;
		renderPass.clearDepth = true;
		renderPass.clearStencil = true;
		renderPass.overrideShader = CRenderer::GetShader<DepthFromLightShader>();
		renderPass.pass = Pass::Lightmap;
		renderPass.viewPort = lightDepthTexture->GetViewPort();
		renderPass.depthStencilView = lightDepthTexture->GetDepthStencilView();
		CManager::AddRenderPass(renderPass);

		// create portal render textures
		auto bluePortalTexture = std::make_shared<RenderTexture>(3, SCREEN_WIDTH, SCREEN_HEIGHT, false);
		CRenderer::BindRenderTargetView(bluePortalTexture);

		auto blueTempPortalTexture = std::make_shared<RenderTexture>(4, SCREEN_WIDTH, SCREEN_HEIGHT, false);
		CRenderer::BindRenderTargetView(blueTempPortalTexture);
		PortalManager::BindRenderTexture(PortalType::Blue, bluePortalTexture, blueTempPortalTexture);

		auto orangePortalTexture = std::make_shared<RenderTexture>(5, SCREEN_WIDTH, SCREEN_HEIGHT, false);
		CRenderer::BindRenderTargetView(orangePortalTexture);

		auto orangeTempPortalTexture = std::make_shared<RenderTexture>(6, SCREEN_WIDTH, SCREEN_HEIGHT, false);
		CRenderer::BindRenderTargetView(orangeTempPortalTexture);
		PortalManager::BindRenderTexture(PortalType::Orange, orangePortalTexture, orangeTempPortalTexture);

		// render to portal texture (blue portal)
		renderPass = {};
		renderPass.targetOutput = { bluePortalTexture->GetRenderTargetViewID(), blueTempPortalTexture->GetRenderTargetViewID() };
		renderPass.clearRTV = true;
		renderPass.clearDepth = renderPass.clearStencil = true;
		renderPass.pass = Pass::PortalBlue;
		CManager::AddRenderPass(renderPass);

		for (int i = 1; i <= m_recursionNum; ++i)
		{
			if (i % 2 == 0)
				renderPass.targetOutput = { bluePortalTexture->GetRenderTargetViewID() };
			else
				renderPass.targetOutput = { blueTempPortalTexture->GetRenderTargetViewID() };

			CManager::AddRenderPass(renderPass);
		}

		// render to portal texture (orange portal)
		renderPass.targetOutput = { orangePortalTexture->GetRenderTargetViewID(), orangeTempPortalTexture->GetRenderTargetViewID() };
		renderPass.pass = Pass::PortalOrange;
		CManager::AddRenderPass(renderPass);

		for (int i = 1; i <= m_recursionNum; ++i)
		{
			if (i % 2 == 0)
				renderPass.targetOutput = { orangePortalTexture->GetRenderTargetViewID() };
			else
				renderPass.targetOutput = { orangeTempPortalTexture->GetRenderTargetViewID() };

			CManager::AddRenderPass(renderPass);
		}

		// finally draw everything
		renderPass = {};
		renderPass.targetOutput = { 1 };
		renderPass.pass = Pass::Default;
		renderPass.overrideShader = nullptr;
		renderPass.clearRTV = true;
		renderPass.clearDepth = renderPass.clearStencil = true;
		CManager::AddRenderPass(renderPass);
	}
	// setup render passes for rendering with stencil
	else
	{
		// create lightmap
		auto lightDepthTexture = std::make_shared<RenderTexture>(2, 2048, 2048, true);
		CRenderer::BindRenderTargetView(lightDepthTexture);

		// write to lightmap in the first pass
		RenderPass renderPass = {};
		renderPass.targetOutput = { lightDepthTexture->GetRenderTargetViewID() };
		renderPass.clearRTV = true;
		renderPass.clearDepth = true;
		renderPass.clearStencil = true;
		renderPass.overrideShader = CRenderer::GetShader<DepthFromLightShader>();
		renderPass.pass = Pass::Lightmap;
		renderPass.viewPort = lightDepthTexture->GetViewPort();
		renderPass.depthStencilView = lightDepthTexture->GetDepthStencilView();
		CManager::AddRenderPass(renderPass);

		// write stencil inside blue portal
		renderPass = {};
		renderPass.targetOutput = { 1 };
		renderPass.pass = Pass::PortalBlue;
		renderPass.overrideShader = CRenderer::GetShader<PortalStencilShader>();
		renderPass.clearRTV = true;
		renderPass.clearStencil = true;
		renderPass.clearDepth = true;
		CManager::AddRenderPass(renderPass);

		// inside blue portal
		renderPass.overrideShader = nullptr;
		renderPass.clearRTV = false;
		renderPass.clearStencil = false;
		renderPass.clearDepth = true;
		for (int i = 0; i <= m_recursionNum; ++i)
			CManager::AddRenderPass(renderPass);

		// render portal blue frame
		renderPass.pass = Pass::PortalBlueFrame;
		renderPass.overrideShader = CRenderer::GetShader<PortalStencilShader>();
		CManager::AddRenderPass(renderPass);

		// write stencil inside orange portal
		renderPass.pass = Pass::PortalOrange;
		renderPass.overrideShader = CRenderer::GetShader<PortalStencilShader>();
		renderPass.clearStencil = true;
		renderPass.clearDepth = true;
		CManager::AddRenderPass(renderPass);

		// inside orange portal
		renderPass.overrideShader = nullptr;
		renderPass.clearStencil = false;
		renderPass.clearDepth = true;
		for (int i = 0; i <= m_recursionNum; ++i)
			CManager::AddRenderPass(renderPass);

		// render portal orange frame
		renderPass.pass = Pass::PortalOrangeFrame;
		renderPass.overrideShader = CRenderer::GetShader<PortalStencilShader>();
		CManager::AddRenderPass(renderPass);

		// draw both portals and frames into depth
		renderPass.overrideShader = CRenderer::GetShader<PortalStencilShader>();
		renderPass.pass = Pass::Default;
		renderPass.clearDepth = true;
		renderPass.clearStencil = true;
		CManager::AddRenderPass(renderPass);

		// draw everthing normally
		renderPass.overrideShader = nullptr;
		renderPass.pass = Pass::Default;
		renderPass.clearDepth = false;
		renderPass.clearStencil = false;
		CManager::AddRenderPass(renderPass);
	}
}
