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

	m_position = dx::XMFLOAT3(40.0F, 1.2F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0f, 0, 0.0f);
	m_scale = dx::XMFLOAT3(0.2F, 0.2F, 0.2F);

	m_enableFrustumCulling = false;

	m_obb.Init((GameObject*)this, 10, 10, 10, 0, 0, -5);
}

void Cube::Init()
{
	GameObject::Init();

	PortalManager::AddPortalObject(CManager::GetActiveScene()->GetSharedPointer(0, this));
}

void Cube::Uninit()
{
	GameObject::Uninit();
}

void Cube::Update()
{
	GameObject::Update();

	m_obb.Update();
	Movement();
}

void Cube::Movement()
{
	// normalized wasd movement
	dx::XMVECTOR moveDirection = dx::XMVectorZero();
	if (CInput::GetKeyPress(DIK_UPARROW))
		moveDirection = dx::XMVectorAdd(moveDirection, {0,0,0.1f});
	if (CInput::GetKeyPress(DIK_LEFTARROW))
		moveDirection = dx::XMVectorAdd(moveDirection, {-0.1f,0,0});
	if (CInput::GetKeyPress(DIK_DOWNARROW))
		moveDirection = dx::XMVectorAdd(moveDirection, {0,0,-0.1f});
	if (CInput::GetKeyPress(DIK_RIGHTARROW))
		moveDirection = dx::XMVectorAdd(moveDirection, { 0.1f,0,0 });

	dx::XMFLOAT3 tempVel;
	dx::XMStoreFloat3(&tempVel, moveDirection);
	m_position += tempVel;
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
		dx::XMMATRIX world = GetClonedWorldMatrix();
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
		// get the orientation from the clone and update the current orientation with the cloned one
		dx::XMMATRIX matrix = portal->GetClonedOrientationMatrix(GetWorldMatrix());
		dx::XMFLOAT4X4 t;
		dx::XMStoreFloat4x4(&t, matrix);

		dx::XMFLOAT3 clonedPos = { t._41, t._42, t._43 };
		dx::XMFLOAT3 clonedForward = { t._31, t._32, t._33 };
		dx::XMFLOAT3 clonedUp = { t._21, t._22, t._23 };

		// swap up, velocity, forward and position
		//virtualUp = clonedUp;

		//dx::XMVECTOR vel = dx::XMLoadFloat3(&m_velocity);
		//dx::XMStoreFloat3(&m_velocity, portal->GetClonedVelocity(vel));

		m_position = clonedPos;
	}
}

dx::XMMATRIX Cube::GetClonedWorldMatrix() const
{
	if (auto portal = PortalManager::GetPortal(m_entrancePortal))
	{
		// get the cloned position and return
		return portal->GetClonedOrientationMatrix(GetWorldMatrix());
	}

	return dx::XMMatrixIdentity();
}
