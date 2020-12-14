#include "pch.h"
#include "manager.h"
#include "renderer.h"
#include "modelmanager.h"
#include "player.h"
#include "math.h"
#include "input.h"
#include "main.h"
#include "stage.h"
#include "reloadui.h"
#include "terrain.h"
#include "light.h"
#include "rendertexture.h"
#include "fpscamera.h"


void Player::Awake()
{
	GameObject::Awake();

	// init player stuff
	m_shader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_PLAYER, m_model);

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(0.06F, 0.06F, 0.06F);

	m_obb.Init((GameObject*)this, 200, 200, 200);
	m_moveSpeed = 0.3F;
	m_titleDisplay = false;
	m_enableFrustumCulling = false;

	m_model->Update(0, 0);
}

void Player::Init()
{
	GameObject::Init();

	if(!m_titleDisplay)
	{ 
		auto thisgo = CManager::GetActiveScene()->GetGameObjects<Player>(0).front();
		std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera())->SetFollowTarget(thisgo);
	}
}

void Player::Uninit()
{
	GameObject::Uninit();
}

void Player::Update()
{
	GameObject::Update();

	static float frame = 0;
	frame += 0.2F;

	// in title display mode
	if (m_titleDisplay)
	{
		m_model->Update(frame, 0);
		return;
	}

	// animation
	//else if (CInput::GetKeyPress(DIK_W) || CInput::GetKeyPress(DIK_A) || CInput::GetKeyPress(DIK_S) || CInput::GetKeyPress(DIK_D))
	//	m_model->Update(frame, 1);
	//else
	//	m_model->Update(frame, 0);

	// movement and collision
	Movement();

	//m_obb.Update();
	//auto enemy = CManager::GetActiveScene()->GetGameObjects<Enemy>(0).front();
	//m_intersectVector = m_obb.CheckObbCollision(&enemy->obb);
	//m_position += m_intersectVector;

	if (m_position.y < 0 || m_position.y > 0) m_position.y = 0;

	// shoot portal
	if (CInput::GetMouseLeftTrigger())
		ShootPortal(PortalType::Blue);
	else if (CInput::GetMouseRightTrigger())
		ShootPortal(PortalType::Orange);
}

void Player::Draw(Pass pass)
{
	GameObject::Draw(pass);

	// title rendering
	if (m_titleDisplay)
	{
		dx::XMMATRIX world = GetWorldMatrix();
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

	// only draw through portal view
	if (!(pass == Pass::PortalBlue || pass == Pass::PortalOrange))
		return;

	// set buffers
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

	world = dx::XMLoadFloat4x4(&t);
	m_shader->SetWorldMatrix(&world);

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
		m_shader->SetViewMatrix(&PortalManager::GetViewMatrix(PortalType::Blue));
		m_shader->SetProjectionMatrix(&PortalManager::GetProjectionMatrix(PortalType::Blue));
	}
	else if (pass == Pass::PortalOrange)
	{
		m_shader->SetViewMatrix(&PortalManager::GetViewMatrix(PortalType::Orange));
		m_shader->SetProjectionMatrix(&PortalManager::GetProjectionMatrix(PortalType::Orange));
	}

	// draw the model
	CRenderer::DrawModel(m_shader, m_model);
	//m_obb.Draw();

	//ImGui::SetNextWindowSize(ImVec2(150, 200));
	//ImGui::Begin("Player Debug");
	//bool colliding = !(m_intersectVector == dx::XMFLOAT3(0, 0, 0));
	//ImGui::Checkbox("is colliding", &colliding);
	//ImGui::Text("intersection vector");
	//ImGui::Text("%.2f, %.2f, %.2f", m_intersectVector.x, m_intersectVector.y, m_intersectVector.z);
	//ImGui::End();
}

void Player::Draw(const std::shared_ptr<Shader>& shader, Pass pass)
{
	GameObject::Draw(shader, pass);

	// set shader buffers
	dx::XMMATRIX world = GetWorldMatrix();
	shader->SetWorldMatrix(&world);

	shader->SetProjectionMatrix(&LightManager::GetDirectionalProjectionMatrix());
	shader->SetViewMatrix(&LightManager::GetDirectionalViewMatrix());

	// draw the model
	CRenderer::DrawModel(shader, m_model);
}

void Player::Movement()
{
	auto forward = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera())->GetForwardVector();
	auto right = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera())->GetRightVector();
	forward = dx::XMVectorSetY(forward, 0);
	forward = dx::XMVector3Normalize(forward);

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
	m_position += dx::XMVectorScale(moveDirection, m_moveSpeed);
}

void Player::ShootPortal(PortalType type)
{
	auto field = CManager::GetActiveScene()->GetGameObjects<Stage>(0).front();
	auto cam = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera());

	dx::XMFLOAT3 point, direction;
	dx::XMStoreFloat3(&point, cam->GetEyePosition());
	dx::XMStoreFloat3(&direction, cam->GetForwardVector());

	dx::XMFLOAT3 pos, normal, up;
	auto colliders = field->GetColliders();
	for (const auto collider : *colliders)
	{
		if (collider->GetLineCollisionPoint(point, direction, pos, normal, up))
		{
			PortalManager::CreatePortal(type, pos, normal, up);
			break;
		}
	}
}
