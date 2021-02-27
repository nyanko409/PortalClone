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
#include <typeinfo>


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
	m_obb.Init((GameObject*)this, 33, 70, 33, 0, 35, 0);

	m_moveSpeed = 0.3f;
	m_grabRadius = 3.5f;
	m_titleDisplay = false;
	m_enableFrustumCulling = false;
	m_isJumping = false;
	m_velocity = m_movementVelocity = { 0,0,0 };
	m_entrancePortal = PortalType::None;

	m_model->Update(0, 0);
}

void Player::Init()
{
	GameObject::Init();

	m_camera = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera());
	PortalManager::AddPortalTraveler(CManager::GetActiveScene()->GetSharedPointer(0, this));
}

void Player::Uninit()
{
	GameObject::Uninit();
}

void Player::Update()
{
	GameObject::Update();

	if (m_titleDisplay)
		return;

	//UpdateAnimation();

	// adjust player virtual up position back to 0,1,0
	if (fabsf(virtualUp.x) > 0.01f || virtualUp.y < 0.99f || fabsf(virtualUp.z) > 0.01f)
	{
		virtualUp = Lerp(virtualUp, dx::XMFLOAT3{ 0,1,0 }, 0.06f);

		auto blue = PortalManager::GetPortal(PortalType::Blue);
		auto orange = PortalManager::GetPortal(PortalType::Orange);
		if (blue && orange && blue->GetForward(true).y >= 0.9f && orange->GetForward(true).y >= 0.9f)
		{
			virtualUp = { 0,1,0 };
		}
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

	// movement, jump
	Movement();
	Jump();
	PortalFunneling();

	// apply gravity
	m_velocity.y -= 0.02f;

	// clamp velocity
	m_velocity.y = Clamp(-1.0f, 10.0f, m_velocity.y);

	// update position
	m_camera->AddPosition(m_velocity + m_movementVelocity);
	SetPosition(m_camera->GetPosition());
	m_position -= virtualUp * m_camera->GetHeight();

	// reduce velocity over time to 0 because of portal velocity
	if(!m_isJumping)
		m_velocity = Lerp(m_velocity, dx::XMFLOAT3{ 0,m_velocity.y,0 }, 0.2f);

	// handle collision
	UpdateCollision();

	// update grabbing object position
	UpdateGrabObject();
	UpdateGrabCollision();

	// toggle grab object
	if (CInput::GetKeyTrigger(DIK_E))
		GrabObject();

	// shoot portal
	if (!m_grabbingObject.lock())
	{
		if (CInput::GetMouseLeftTrigger())
			ShootPortal(PortalType::Blue);
		else if (CInput::GetMouseRightTrigger())
			ShootPortal(PortalType::Orange);
	}
}

void Player::Draw(Pass pass)
{
	GameObject::Draw(pass);

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
			m_shader->SetLightProjectionMatrix(&LightManager::GetProjectionMatrix());
			m_shader->SetLightViewMatrix(&LightManager::GetViewMatrix());
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

	if (pass == Pass::PortalBlue)
	{
		m_shader->SetViewMatrix(&PortalManager::GetViewMatrix(PortalType::Blue));
		m_shader->SetProjectionMatrix(&PortalManager::GetProjectionMatrix(PortalType::Blue));
	}
	else if (pass == Pass::PortalOrange)
	{
		m_shader->SetViewMatrix(&PortalManager::GetViewMatrix(PortalType::Orange));
		m_shader->SetProjectionMatrix(&PortalManager::GetProjectionMatrix(PortalType::Orange));
	}

	// only draw player through portal view and if both portals are active
	if ((pass == Pass::PortalBlue || pass == Pass::PortalOrange))
	{
		if (PortalManager::GetPortal(PortalType::Blue) && PortalManager::GetPortal(PortalType::Orange))
		{
			dx::XMMATRIX world = GetFixedUpWorldMatrix();
			m_shader->SetWorldMatrix(&world);
			CRenderer::DrawModel(m_shader, m_model);
		}
	}
	else if (auto portal = PortalManager::GetPortal(m_entrancePortal))
	{
		// draw the cloned player
		dx::XMMATRIX world = GetClonedWorldMatrix();
		m_shader->SetWorldMatrix(&world);
		CRenderer::DrawModel(m_shader, m_model);
	}

	// draw collider
	m_obb.Draw();
}

void Player::Draw(const std::shared_ptr<Shader>& shader, Pass pass)
{
	if (!(pass == Pass::Lightmap))
		return;

	GameObject::Draw(shader, pass);

	// set shader buffers
	dx::XMMATRIX world = GetFixedUpWorldMatrix();
	shader->SetWorldMatrix(&world);

	shader->SetProjectionMatrix(&LightManager::GetProjectionMatrix());
	shader->SetViewMatrix(&LightManager::GetViewMatrix());

	// draw the model
	CRenderer::DrawModel(shader, m_model);
}

void Player::Swap()
{
	if (auto portal = PortalManager::GetPortal(m_entrancePortal))
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

		dx::XMFLOAT3 adjustedVel = m_velocity;
		if (fabsf(portal->GetForward(true).y) != 1 || fabsf(portal->GetLinkedPortal()->GetForward(true).y) != 1)
			adjustedVel.y *= 1.2f;

		dx::XMVECTOR vel = dx::XMLoadFloat3(&adjustedVel);
		dx::XMStoreFloat3(&m_velocity, portal->GetClonedVelocity(vel));

		m_camera->Swap(clonedForward, clonedPos);
		dx::XMStoreFloat3(&m_position, m_camera->GetPosition());
		m_position -= virtualUp * m_camera->GetHeight();

		// swap the entrance portal
		if (portal->GetType() == PortalType::Blue)
			SetEntrancePortal(PortalType::Orange);
		else
			SetEntrancePortal(PortalType::Blue);
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
		m_velocity.y += 0.4f;
	}
}

void Player::UpdateCollision()
{
	float startY = m_position.y;

	// cube collision, skip if the cube is being grabbed
	auto cube = CManager::GetActiveScene()->GetGameObjectsOfType<Cube>(0).front();

	if (&(*std::static_pointer_cast<GameObject>(cube)) != &(*m_grabbingObject.lock()))
	{
		m_camera->AddPosition(Collision::ObbObbCollision(&m_obb, cube->GetOBB()));
		dx::XMStoreFloat3(&m_position, m_camera->GetPosition());
		m_position -= virtualUp * m_camera->GetHeight();

		// cloned cube collision
		if (auto portal = PortalManager::GetPortal(cube->GetEntrancePortal()))
		{
			cube->GetOBB()->OverrideWorldMatrix(true, portal->GetClonedOrientationMatrix(cube->GetWorldMatrix()));

			m_camera->AddPosition(Collision::ObbObbCollision(&m_obb, cube->GetOBB()));
			dx::XMStoreFloat3(&m_position, m_camera->GetPosition());
			m_position -= virtualUp * m_camera->GetHeight();

			cube->GetOBB()->OverrideWorldMatrix(false);
		}
	}

	// stage collision
	auto stageColliders = CManager::GetActiveScene()->GetGameObjectsOfType<Stage>(0).front()->GetColliders();
	for (const auto& col : *stageColliders)
	{
		// ignore collision on walls attached to the current colliding portal
		if (auto portal = PortalManager::GetPortal(m_entrancePortal))
		{
			if (portal->GetAttachedColliderId() == col->GetId())
				continue;
		}

		m_camera->AddPosition(Collision::ObbPolygonCollision(&m_obb, col));
		dx::XMStoreFloat3(&m_position, m_camera->GetPosition());
		m_position -= virtualUp * m_camera->GetHeight();
	}

	// portal collision
	if (auto portal = PortalManager::GetPortal(m_entrancePortal))
	{
		auto colliders = portal->GetEdgeColliders();
		for (auto col : *colliders)
		{
			m_camera->AddPosition(Collision::ObbObbCollision(&m_obb, col));
			dx::XMStoreFloat3(&m_position, m_camera->GetPosition());
			m_position -= virtualUp * m_camera->GetHeight();
		}
	}

	// check if player landed on something
	if (startY < m_position.y)
	{
		m_velocity.y = 0;
		m_isJumping = false;
	}
	else
	{
		m_isJumping = true;
	}
}

void Player::ShootPortal(PortalType type)
{
	if (m_camera->InDebugMode())
		return;

	auto stage = CManager::GetActiveScene()->GetGameObjectsOfType<Stage>(0).front();

	dx::XMFLOAT3 point, direction;
	dx::XMStoreFloat3(&point, m_camera->GetPosition());
	dx::XMStoreFloat3(&direction, m_camera->GetForwardVector());
	dx::XMVECTOR a = dx::XMLoadFloat3(&point);

	// get the nearest collider hit
	float nearestDist = std::numeric_limits<float>().max();
	int id = -1;
	dx::XMFLOAT3 outPos, outFinalPos, outNormal, outFinalNormal, outUp, outFirnalUp;
	auto colliders = stage->GetColliders();
	for (const auto& collider : *colliders)
	{
		if(Collision::LinePolygonCollision(collider, point, direction, outPos, outNormal, outUp))
		{
			// cache the nearest collider hit
			dx::XMVECTOR b = dx::XMLoadFloat3(&outPos);
			float dist = dx::XMVectorGetX(dx::XMVector3LengthSq(dx::XMVectorSubtract(b, a)));
			if (dist < nearestDist)
			{
				nearestDist = dist;
				id = collider->GetId();
				outFinalPos = outPos;
				outFinalNormal = outNormal;
				outFirnalUp = outUp;
			}
		}
	}

	// if hit, create portal
	if(id != -1)
		PortalManager::CreatePortal(type, outFinalPos, outFinalNormal, outFirnalUp, id);
}

void Player::GrabObject()
{
	if (auto grab = m_grabbingObject.lock())
	{
		// if already holding a object, drop the object
		grab->EnableUpdate(true);
		grab->SetVelocity({ 0,0,0 });
		m_grabbingObject.reset();
	}
	else
	{
		// get grabbable objects in scene
		auto grabbables = CManager::GetActiveScene()->GetGameObjectsOfTypeNoCast<Grabbable>(0);
		for (auto grabbable : grabbables)
		{
			// check if its in radius
			auto diff = dx::XMVectorSubtract(grabbable->GetPosition(), GetGrabPosition());
			float lengthSq = dx::XMVectorGetX(dx::XMVector3LengthSq(diff));
			if (lengthSq < m_grabRadius * m_grabRadius)
			{
				m_grabbingObject = grabbable;
				grabbable->EnableUpdate(false);
				break;
			}

			// check for other side of portal
			if (auto portal = PortalManager::GetPortal(m_entrancePortal))
			{
				auto clonedPos = portal->GetClonedPosition(GetGrabPosition());
				auto diff = dx::XMVectorSubtract(grabbable->GetPosition(), clonedPos);
				float lengthSq = dx::XMVectorGetX(dx::XMVector3LengthSq(diff));
				if (lengthSq < m_grabRadius * m_grabRadius)
				{
					m_grabbingObject = grabbable;
					grabbable->EnableUpdate(false);
					break;
				}
			}
		}
	}
}

void Player::UpdateGrabObject()
{
	if (auto obj = m_grabbingObject.lock())
	{
		auto traveler = std::dynamic_pointer_cast<PortalTraveler>(obj);
		auto point = GetGrabPosition();
		
		if (auto portal = PortalManager::GetPortal(m_entrancePortal))
		{
			// player is near the portal
			if (portal->GetLinkedPortal())
			{
				dx::XMVECTOR portalForward = dx::XMLoadFloat3(&portal->GetForward());
				dx::XMVECTOR portalToPoint = dx::XMVectorSubtract(point, portal->GetPosition());
		
				float dot = dx::XMVectorGetX(dx::XMVector3Dot(portalForward, portalToPoint));
				if (dot < 0.0f)
				{
					// point is on the other side
					if (m_entrancePortal != traveler->GetEntrancePortal())
						obj->SetPosition(portal->GetClonedPosition(point));
					else
						obj->SetPosition(point);
				}
				else
				{
					// point is still on the same side
					if (m_entrancePortal == traveler->GetEntrancePortal())
						obj->SetPosition(point);
					else if (traveler->GetEntrancePortal() != PortalType::None)
						obj->SetPosition(portal->GetClonedPosition(point));
					else
						obj->SetPosition(point);
				}
			}
		}
		else
		{
			obj->SetPosition(point);
		}
	}
}

void Player::UpdateGrabCollision()
{
	if (auto obj = m_grabbingObject.lock())
	{
		auto grab = std::dynamic_pointer_cast<PortalTraveler>(obj);

		// stage collision
		auto stageColliders = CManager::GetActiveScene()->GetGameObjectsOfType<Stage>(0).front()->GetColliders();
		for (const auto& col : *stageColliders)
		{
			// ignore collision on walls attached to the current colliding portal
			if (auto portal = PortalManager::GetPortal(grab->GetEntrancePortal()))
			{
				if (portal->GetAttachedColliderId() == col->GetId())
					continue;
			}

			obj->AddPosition(Collision::ObbPolygonCollision(grab->GetOBB(), col));
		}

		// if not near a portal, do proper collision response
		if (grab->GetEntrancePortal() == PortalType::None && GetEntrancePortal() == PortalType::None)
		{
			dx::XMFLOAT3 forward, position;
			dx::XMStoreFloat3(&forward, dx::XMVector3Normalize(dx::XMVectorSubtract(obj->GetPosition(), m_camera->GetPosition())));
			dx::XMStoreFloat3(&position, m_camera->GetPosition());
			m_camera->Swap(forward, position);
		}
		else
		{
			// else do portal collision
			if (auto portal = PortalManager::GetPortal(grab->GetEntrancePortal()))
			{
				auto colliders = portal->GetEdgeColliders();
				for (auto col : *colliders)
				{
					obj->AddPosition(Collision::ObbObbCollision(grab->GetOBB(), col));
				}
			}
		}
	}
}

void Player::PortalFunneling()
{
	// return if player is trying to escape
	if (m_movementVelocity.x != 0 || m_movementVelocity.z != 0 || !m_isJumping)
		return;

	// check if both portals normals are up or down
	dx::XMFLOAT3 normal;
	float maxDiff = 1.0f;

	if (auto entrance = PortalManager::GetPortal(m_entrancePortal))
	{
		normal = entrance->GetForward();
		if (fabsf(normal.x) < 0.01f && fabsf(normal.z) < 0.01f)
		{
			if (auto exit = PortalManager::GetPortal(m_entrancePortal)->GetLinkedPortal())
			{
				normal = exit->GetForward();
				if (fabsf(normal.x) < 0.01f && fabsf(normal.z) < 0.01f)
				{
					// move the player closer to portal if both portals are close on the xz plane
					dx::XMFLOAT3 pos1 = entrance->GetPositionFloat();
					dx::XMFLOAT3 pos2 = exit->GetPositionFloat();
					if (fabsf(pos1.x - pos2.x) <= maxDiff && fabsf(pos1.z - pos2.z) <= maxDiff)
					{
						dx::XMFLOAT3 lerpPos = entrance->GetPositionFloat();
						lerpPos.y = m_camera->GetPositionFloat().y;

						m_camera->SetPosition(Lerp(m_camera->GetPositionFloat(), lerpPos, 0.2f));
						dx::XMStoreFloat3(&m_position, m_camera->GetPosition());
						m_position -= virtualUp * m_camera->GetHeight();
					}
				}
			}
		}
	}
}

dx::XMVECTOR Player::GetGrabPosition() const
{
	return dx::XMVectorAdd(m_camera->GetPosition(), dx::XMVectorScale(m_camera->GetForwardVector(), 4));
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
	if (auto portal = PortalManager::GetPortal(m_entrancePortal))
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
