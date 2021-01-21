#include "pch.h"
#include "manager.h"
#include "modelmanager.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "cube.h"
#include "math.h"
#include "input.h"
#include "main.h"
#include "fpscamera.h"
#include "frustumculling.h"
#include "light.h"
#include "rendertexture.h"


void Cube::Awake()
{
	GameObject::Awake();

	// init player stuff
	m_shader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_CUBE, m_model);

	SetPosition(0.0F, 1.2F, 40.0F);
	SetRotation(0, 0, 0);
	m_scale = dx::XMFLOAT3(0.2F, 0.2F, 0.2F);

	m_entrancePortal = PortalType::None;
	m_enableFrustumCulling = false;

	m_obb.Init((GameObject*)this, 10, 10, 10, 0, 0, 0);
}

void Cube::Init()
{
	GameObject::Init();

	PortalManager::AddPortalTraveler(CManager::GetActiveScene()->GetSharedPointer(0, this));
}

void Cube::Uninit()
{
	GameObject::Uninit();
}

void Cube::Update()
{
	GameObject::Update();

	AddRotation(GetForward(true), 1);

	m_obb.Update();
	Movement();
}

void Cube::Movement()
{
	// normalized wasd movement
	dx::XMFLOAT3 moveDirection = { 0,0,0 };
	auto forward = GetForward(true);
	auto right = GetRight(true);
	if (CInput::GetKeyPress(DIK_UPARROW))
		moveDirection += forward;
	if (CInput::GetKeyPress(DIK_LEFTARROW))
		moveDirection -= right;
	if (CInput::GetKeyPress(DIK_DOWNARROW))
		moveDirection -= forward;
	if (CInput::GetKeyPress(DIK_RIGHTARROW))
		moveDirection += right;

	moveDirection = moveDirection * 0.1f;
	AddPosition(moveDirection);
}

void Cube::Draw(Pass pass)
{
	if (!(pass == Pass::Default || pass == Pass::PortalBlue || pass == Pass::PortalOrange))
		return;

	GameObject::Draw(pass);
	m_obb.Draw();

	// set buffers
	m_shader->SetDirectionalLight(LightManager::GetDirectionalLight());

	dx::XMMATRIX world = GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);

	MATERIAL mat = {};
	mat.Diffuse = { 1,1,1,1 };
	mat.Specular = { 1,1,1,1 };
	m_shader->SetMaterial(mat);

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

	// draw the cloned model
	if (auto portal = PortalManager::GetPortal(m_entrancePortal))
	{
		dx::XMMATRIX world = portal->GetClonedOrientationMatrix(GetWorldMatrix());
		m_shader->SetWorldMatrix(&world);
		CRenderer::DrawModel(m_shader, m_model);
	}
}

void Cube::Draw(const std::shared_ptr<Shader>& shader, Pass pass)
{
	if (pass == Pass::StencilOnly)
		return;

	GameObject::Draw(shader, pass);

	// set shader buffers
	dx::XMMATRIX world = GetWorldMatrix();
	shader->SetWorldMatrix(&world);

	shader->SetProjectionMatrix(&LightManager::GetDirectionalProjectionMatrix());
	shader->SetViewMatrix(&LightManager::GetDirectionalViewMatrix());

	// draw the model
	CRenderer::DrawModel(shader, m_model);
}

void Cube::Swap()
{
	if (auto portal = PortalManager::GetPortal(m_entrancePortal))
	{
		// get the cloned orientation
		dx::XMVECTOR clonedScale, clonedRot, clonedPos;
		dx::XMMatrixDecompose(&clonedScale, &clonedRot, &clonedPos, portal->GetClonedOrientationMatrix(GetWorldMatrix()));

		// swap the scale, rotation and position
		SetScale(clonedScale);
		SetRotation(clonedRot);
		SetPosition(clonedPos);

		// swap the velocity
		//dx::XMVECTOR vel = dx::XMLoadFloat3(&m_velocity);
		//dx::XMStoreFloat3(&m_velocity, portal->GetClonedVelocity(vel));
	}
}
