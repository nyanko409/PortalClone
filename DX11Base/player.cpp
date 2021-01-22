#include "pch.h"
#include "manager.h"
#include "renderer.h"
#include "modelmanager.h"
#include "player.h"
#include "math.h"
#include "input.h"
#include "main.h"
#include "stage.h"
#include "terrain.h"
#include "light.h"
#include "rendertexture.h"
#include "fpscamera.h"
#include "cube.h"


void Player::Awake()
{
	GameObject::Awake();

	// init player stuff
	m_shader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_PLAYER, m_model);

	SetPosition(0.0F, 0.0F, 0.0F);
	SetRotation(0.0F, 0.0F, 0.0F);
	SetScale(0.06F, 0.06F, 0.06F);

	virtualUp = { 0, 1, 0 };
	m_obb.Init((GameObject*)this, 40, 70, 40, 0, 35, 0);

	m_moveSpeed = 0.3F;
	m_titleDisplay = false;
	m_enableFrustumCulling = false;
	m_isJumping = false;
	m_velocity = m_movementVelocity = { 0,0,0 };
	m_entrancePortal = PortalStencilType::None;

	m_model->Update(0, 0);
}

void Player::Init()
{
	GameObject::Init();

	m_camera = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera());
	PortalStencilManager::AddPortalTraveler(CManager::GetActiveScene()->GetSharedPointer(0, this));
}

void Player::Uninit()
{
	GameObject::Uninit();
}

void Player::Update()
{
	GameObject::Update();

	//UpdateAnimation();

	// adjust player virtual up position back to 0,1,0
	static float rot = 0;
	if (fabsf(virtualUp.x) > 0.01f || virtualUp.y < 0.99f || fabsf(virtualUp.z) > 0.01f)
	{
		virtualUp = Lerp(virtualUp, dx::XMFLOAT3{ 0,1,0 }, 0.06f);
		//if (rot < 30)
		//{
		//	auto a = dx::XMLoadFloat3(&virtualUp);
		//	a = dx::XMVector3TransformNormal(a, dx::XMMatrixRotationAxis({ 0,0,1 }, dx::XMConvertToRadians(6)));
		//	rot++;
		//	dx::XMStoreFloat3(&virtualUp, a);
		//}
		//else
		//	virtualUp = Lerp(virtualUp, dx::XMFLOAT3{ 0,1,0 }, 1.0f);
	}
	else
		rot = 0;

	// movement, jump
	Movement();
	Jump();

	// apply gravity
	m_velocity.y -= 0.05f;

	// clamp velocity
	if (m_velocity.y < -1.2f)
		m_velocity.y = -1.2f;

	// update position
	m_camera->AddPosition(m_velocity + m_movementVelocity);
	SetPosition(m_camera->GetPosition());
	m_position -= virtualUp * m_camera->GetHeight();

	// reduce velocity over time to 0 because of portal velocity
	m_velocity = Lerp(m_velocity, dx::XMFLOAT3{ 0,0,0 }, 0.04f);

	// collision
	UpdateCollision();

	// shoot portal
	if (CInput::GetMouseLeftTrigger())
		ShootPortal(PortalStencilType::Blue);
	else if (CInput::GetMouseRightTrigger())
		ShootPortal(PortalStencilType::Orange);
}

void Player::Draw(Pass pass)
{
	GameObject::Draw(pass);
	m_obb.Draw();

	// title rendering
	if (m_titleDisplay)
	{
		dx::XMMATRIX world = GameObject::GetWorldMatrix();
		m_shader->SetWorldMatrix(&world);

		m_shader->SetDirectionalLight(LightManager::GetDirectionalLight());

		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = dx::XMFLOAT4(1, 1, 1, 1);
		m_shader->SetMaterial(material);

		auto renderTex = CRenderer::GetRenderTexture(2);
		if (renderTex)
		{
			m_shader->SetShadowMapTexture(CRenderer::GetRenderTexture(2)->GetRenderTexture());
			m_shader->SetLightProjectionMatrix(&LightManager::GetDirectionalProjectionMatrix());
			m_shader->SetLightViewMatrix(&LightManager::GetDirectionalViewMatrix());
		}

		// draw the model
		CRenderer::DrawModel(m_shader, m_model);
		return;
	}

	// set buffers
	m_shader->SetDirectionalLight(LightManager::GetDirectionalLight());

	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = dx::XMFLOAT4(1, 1, 1, 1);
	m_shader->SetMaterial(material);

	m_shader->SetShadowMapTexture(CRenderer::GetRenderTexture(2)->GetRenderTexture());
	m_shader->SetLightProjectionMatrix(&LightManager::GetDirectionalProjectionMatrix());
	m_shader->SetLightViewMatrix(&LightManager::GetDirectionalViewMatrix());

	if (pass == Pass::PortalBlue)
	{
		m_shader->SetViewMatrix(&PortalStencilManager::GetViewMatrix(PortalStencilType::Blue));
		m_shader->SetProjectionMatrix(&PortalStencilManager::GetProjectionMatrix(PortalStencilType::Blue));
	}
	else if (pass == Pass::PortalOrange)
	{
		m_shader->SetViewMatrix(&PortalStencilManager::GetViewMatrix(PortalStencilType::Orange));
		m_shader->SetProjectionMatrix(&PortalStencilManager::GetProjectionMatrix(PortalStencilType::Orange));
	}

	// only draw player through portal view
	if ((pass == Pass::PortalBlue || pass == Pass::PortalOrange))
	{
		dx::XMMATRIX world = GetFixedUpWorldMatrix();
		m_shader->SetWorldMatrix(&world);
		CRenderer::DrawModel(m_shader, m_model);
	}

	// draw cloned player
	if (auto portal = PortalStencilManager::GetPortal(m_entrancePortal))
	{
		//dx::XMMATRIX world = GetClonedWorldMatrix();
		//m_shader->SetWorldMatrix(&world);
		//CRenderer::DrawModel(m_shader, m_model);
	}
}

void Player::Draw(const std::shared_ptr<Shader>& shader, Pass pass)
{
	if (pass == Pass::StencilOnly)
		return;

	GameObject::Draw(shader, pass);

	// set shader buffers
	dx::XMMATRIX world = GetFixedUpWorldMatrix();
	shader->SetWorldMatrix(&world);

	shader->SetProjectionMatrix(&LightManager::GetDirectionalProjectionMatrix());
	shader->SetViewMatrix(&LightManager::GetDirectionalViewMatrix());

	// draw the model
	CRenderer::DrawModel(shader, m_model);
}

void Player::Swap()
{
	if (auto portal = PortalStencilManager::GetPortal(m_entrancePortal))
	{
		// get the orientation from the clone and update the current orientation with the cloned one
		dx::XMMATRIX matrix = portal->GetClonedOrientationMatrix(m_camera->GetLocalToWorldMatrix(false));
		dx::XMFLOAT4X4 t;
		dx::XMStoreFloat4x4(&t, matrix);

		dx::XMFLOAT3 clonedPos = { t._41, t._42, t._43 };
		dx::XMFLOAT3 clonedForward = { t._31, t._32, t._33 };

		// update the up vector from fixed up clone matrix
		matrix = portal->GetClonedOrientationMatrix(m_camera->GetLocalToWorldMatrix(true));
		dx::XMStoreFloat4x4(&t, matrix);

		dx::XMFLOAT3 clonedUp = { t._21, t._22, t._23 };

		// swap up, velocity, camera forward and position
		virtualUp = clonedUp;

		dx::XMVECTOR vel = dx::XMLoadFloat3(&m_velocity);
		dx::XMStoreFloat3(&m_velocity, portal->GetClonedVelocity(vel));

		m_camera->Swap(clonedForward, clonedPos);
		dx::XMStoreFloat3(&m_position, m_camera->GetPosition());
		m_position -= virtualUp * m_camera->GetHeight();
	}
}

dx::XMVECTOR Player::GetTravelerPosition() const
{
	return CManager::GetActiveScene()->GetMainCamera()->GetPosition();
}

void Player::UpdateAnimation()
{
	// animation
	static float frame = 0;
	frame += 0.2F;

	// in title display mode
	if (m_titleDisplay)
	{
		//m_model->Update(frame, 0);
		return;
	}

	if (CInput::GetKeyPress(DIK_W) || CInput::GetKeyPress(DIK_A) || CInput::GetKeyPress(DIK_S) || CInput::GetKeyPress(DIK_D))
		m_model->Update(frame, 1);
	else
		m_model->Update(frame, 0);
}

void Player::Movement()
{
	auto forward = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera())->GetForwardVector();
	auto right = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera())->GetRightVector();
	forward = dx::XMVectorSetY(forward, 0);
	right = dx::XMVectorSetY(right, 0);

	// normalized wasd movement
	dx::XMVECTOR moveDirection = dx::XMVectorZero();
	if (CInput::GetKeyPress(DIK_W))
		moveDirection = dx::XMVectorAdd(moveDirection, forward);
	if (CInput::GetKeyPress(DIK_A))
		moveDirection = dx::XMVectorSubtract(moveDirection, right);
	if (CInput::GetKeyPress(DIK_S))
		moveDirection = dx::XMVectorSubtract(moveDirection, forward);
	if (CInput::GetKeyPress(DIK_D))
		moveDirection = dx::XMVectorAdd(moveDirection, right);

	moveDirection = dx::XMVector3Normalize(moveDirection);

	dx::XMFLOAT3 tempVel;
	dx::XMStoreFloat3(&tempVel, dx::XMVectorScale(moveDirection, m_moveSpeed));
	m_movementVelocity.x = tempVel.x;
	m_movementVelocity.z = tempVel.z;
}

void Player::Jump()
{
	// add y velocity on jump
	if (!m_isJumping && CInput::GetKeyTrigger(DIK_SPACE))
	{
		m_isJumping = true;
		m_velocity.y += 0.9f;
	}
}

void Player::UpdateCollision()
{
	m_obb.Update();
	dx::XMFLOAT3 intersection = { 0,0,0 };

	// cube collision
	auto cube = CManager::GetActiveScene()->GetGameObjects<Cube>(0).front();
	intersection += Collision::ObbObbCollision(&m_obb, cube->GetOBB());

	// stage collision
	auto stageColliders = CManager::GetActiveScene()->GetGameObjects<Stage>(0).front()->GetColliders();
	for (const auto& col : *stageColliders)
	{
		// ignore collision on walls attached to the current colliding portal
		if (auto portal = PortalStencilManager::GetPortal(m_entrancePortal))
		{
			if (portal->GetAttachedColliderId() == col->GetId())
				continue;
		}

		intersection += Collision::ObbPolygonCollision(&m_obb, col);
	}

	// portal collision
	if (auto portal = PortalStencilManager::GetPortal(m_entrancePortal))
	{
		auto colliders = portal->GetEdgeColliders();
		for (auto col : *colliders)
		{
			intersection += Collision::ObbObbCollision(&m_obb, col);
		}
	}

	// apply collision offset
	m_camera->AddPosition(intersection);
	dx::XMStoreFloat3(&m_position, m_camera->GetPosition());
	m_position -= virtualUp * m_camera->GetHeight();

	// check if player landed on something
	if (intersection.y > 0.0f)
	{
		m_velocity.y = 0;
		m_isJumping = false;
	}
}

void Player::ShootPortal(PortalStencilType type)
{
	auto cam = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera());
	if (cam->InDebugMode())
		return;

	auto stage = CManager::GetActiveScene()->GetGameObjects<Stage>(0).front();

	dx::XMFLOAT3 point, direction;
	dx::XMStoreFloat3(&point, cam->GetPosition());
	dx::XMStoreFloat3(&direction, cam->GetForwardVector());

	dx::XMFLOAT3 outPos, outNormal, outUp;
	auto colliders = stage->GetColliders();
	for (const auto& collider : *colliders)
	{
		if(Collision::LinePolygonCollision(collider, point, direction, outPos, outNormal, outUp))
		{
			PortalStencilManager::CreatePortal(type, outPos, outNormal, outUp, collider->GetId());
			break;
		}
	}
}

dx::XMMATRIX Player::GetFixedUpWorldMatrix() const
{
	auto right = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera())->GetRightVector();
	dx::XMMATRIX world = GetWorldMatrix();
	dx::XMFLOAT4X4 t;
	dx::XMStoreFloat4x4(&t, world);

	dx::XMVECTOR up = dx::XMVectorSet(0, 1, 0, 0);
	dx::XMVECTOR xaxis = dx::XMVector3Normalize(right);
	dx::XMVECTOR zaxis = dx::XMVector3Normalize(dx::XMVector3Cross(xaxis, up));
	dx::XMVECTOR yaxis = dx::XMVector3Cross(zaxis, xaxis);

	dx::XMFLOAT3 z, x, y;
	dx::XMStoreFloat3(&z, zaxis);
	dx::XMStoreFloat3(&x, xaxis);
	dx::XMStoreFloat3(&y, yaxis);

	t._11 = x.x * m_scale.x;
	t._12 = x.y * m_scale.x;
	t._13 = x.z * m_scale.x;
	t._21 = y.x * m_scale.y;
	t._22 = y.y * m_scale.y;
	t._23 = y.z * m_scale.y;
	t._31 = z.x * m_scale.z;
	t._32 = z.y * m_scale.z;
	t._33 = z.z * m_scale.z;

	return dx::XMLoadFloat4x4(&t);
}

dx::XMMATRIX Player::GetClonedWorldMatrix() const
{
	if (auto portal = PortalStencilManager::GetPortal(m_entrancePortal))
	{
		// get the camera matrix and subtract camera height to get the player position
		dx::XMMATRIX matrix = m_camera->GetLocalToWorldMatrix(true);
		dx::XMFLOAT4X4 t;
		dx::XMStoreFloat4x4(&t, matrix);
		t._42 -= m_camera->GetHeight();
		matrix = dx::XMLoadFloat4x4(&t);

		// get the cloned matrix and return
		matrix = portal->GetClonedOrientationMatrix(matrix);
		dx::XMStoreFloat4x4(&t, matrix);

		t._11 *= m_scale.x;
		t._12 *= m_scale.x;
		t._13 *= m_scale.x;
		t._21 *= m_scale.y;
		t._22 *= m_scale.y;
		t._23 *= m_scale.y;
		t._31 *= m_scale.z;
		t._32 *= m_scale.z;
		t._33 *= m_scale.z;

		return dx::XMLoadFloat4x4(&t);
	}

	return dx::XMMatrixIdentity();
}

dx::XMMATRIX Player::GetWorldMatrix() const
{
	auto right = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera())->GetRightVector();
	dx::XMMATRIX world = GameObject::GetWorldMatrix();
	dx::XMFLOAT4X4 t;
	dx::XMStoreFloat4x4(&t, world);

	dx::XMVECTOR yaxis = dx::XMVector3Normalize(dx::XMLoadFloat3(&virtualUp));
	dx::XMVECTOR xaxis = dx::XMVector3Normalize(right);
	dx::XMVECTOR zaxis = dx::XMVector3Normalize(dx::XMVector3Cross(xaxis, yaxis));

	dx::XMFLOAT3 z, x, y;
	dx::XMStoreFloat3(&z, zaxis);
	dx::XMStoreFloat3(&x, xaxis);
	dx::XMStoreFloat3(&y, yaxis);

	t._11 = x.x * m_scale.x;
	t._12 = x.y * m_scale.x;
	t._13 = x.z * m_scale.x;
	t._21 = y.x * m_scale.y;
	t._22 = y.y * m_scale.y;
	t._23 = y.z * m_scale.y;
	t._31 = z.x * m_scale.z;
	t._32 = z.y * m_scale.z;
	t._33 = z.z * m_scale.z;

	return dx::XMLoadFloat4x4(&t);
}
