#include "pch.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "skybox.h"
#include "math.h"
#include "input.h"
#include "fpscamera.h"


void Skybox::Init()
{
	GameObject::Init();

	// init the skybox
	ModelManager::GetModel(MODEL_SKYBOX, m_model);

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(50.0F, 50.0F, 50.0F);

	m_quaternion = dx::XMFLOAT4(0, 0, 0, 1);
	m_prevRotation = m_rotation;
	m_diffRotation = dx::XMFLOAT3(0, 0, 0);

	center = CManager::GetActiveScene()->GetGameObjects<FPSCamera>(0).front();
}

void Skybox::Uninit()
{
	GameObject::Uninit();
}

void Skybox::Update()
{
	GameObject::Update();
}

void Skybox::Draw()
{
	GameObject::Draw();

	// set the world matrix for this object
	dx::XMVECTOR quaternion = dx::XMLoadFloat4(&m_quaternion);

	dx::XMMATRIX scale, rot, trans;
	scale = dx::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	rot = dx::XMMatrixRotationQuaternion(quaternion);
	trans = dx::XMMatrixTranslationFromVector(center->GetPosition());

	CRenderer::SetWorldMatrix(&(scale * rot * trans));

	// draw the model using the world matrix
	m_model->Draw();

	ImGui::SetNextWindowSize(ImVec2(320, 100));
	ImGui::Begin("Hello World");
	ImGui::Text("dw");
	
	ImGui::End();
}
