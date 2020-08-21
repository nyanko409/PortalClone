#include "pch.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "skybox.h"
#include "math.h"
#include "input.h"
#include "player.h"


void Skybox::Init()
{
	GameObject::Init();

	// init the shader
	m_shader = CRenderer::GetShader<BasicLightShader>();

	// init the skybox
	ModelManager::GetModel(MODEL_SKYBOX, m_model);

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(50.0F, 50.0F, 50.0F);
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

	// set the active shader
	CRenderer::SetShader(m_shader);

	// set the world matrix for this object
	dx::XMVECTOR quaternion = dx::XMLoadFloat4(&m_quaternion);

	dx::XMMATRIX scale, rot, trans;
	scale = dx::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	rot = dx::XMMatrixRotationQuaternion(quaternion);
	trans = dx::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	m_shader->SetWorldMatrix(&(scale * rot * trans));

	// draw the model
	m_model->Draw(m_shader);

	ImGui::SetNextWindowSize(ImVec2(320, 100));
	ImGui::Begin("Hello World");
	ImGui::Text("dw");
	ImGui::End();
}
