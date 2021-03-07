#include "pch.h"
#include "main.h"
#include "titlecamera.h"
#include "shader.h"


void TitleCamera::Init()
{
	Camera::Init();
}

void TitleCamera::Uninit()
{
	Camera::Uninit();
}

void TitleCamera::Draw(Pass pass)
{
	Camera::Draw(pass);
}

void TitleCamera::Update()
{
	Camera::Update();
}

void TitleCamera::SetViewMatrix()
{
	// convert float4x4 to matrices
	dx::XMMATRIX view = dx::XMLoadFloat4x4(&m_mView);
	dx::XMVECTOR up = dx::XMVectorSet(0, 1, 0, 0);
	dx::XMVECTOR eye = GetPosition();
	dx::XMVECTOR target = {0, 1, 0};

	// calculate and set the view matrix for each shader
	view = dx::XMMatrixLookAtLH(eye, target, up);

	auto shaders = CRenderer::GetShaders();
	for (auto shader : shaders)
	{
		shader->SetViewMatrix(&view);
	}

	// load the view matrix back to member variable
	dx::XMStoreFloat4x4(&m_mView, view);
}
