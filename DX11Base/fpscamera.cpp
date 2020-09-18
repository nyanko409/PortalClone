#include "pch.h"
#include "main.h"
#include "fpscamera.h"
#include "input.h"
#include "bullet.h"
#include "manager.h"
#include "billboard.h"


POINT operator - (POINT o1, POINT o2) { return POINT{ o1.x - o2.x, o1.y - o2.y }; }

void FPSCamera::Init()
{
	Camera::Init();

	m_moveSpeed = 0.2F;

	ScreenToClient(GetWindow(), &m_cursorFixedPos);
	m_cursorFixedPos.x = fabsf(m_cursorFixedPos.x) + SCREEN_WIDTH / 2.0F;
	m_cursorFixedPos.y = fabsf(m_cursorFixedPos.y) + SCREEN_HEIGHT / 2.0F;

	m_forward = dx::XMFLOAT3(0, 0, 1);
	m_right = dx::XMFLOAT3(1, 0, 0);

	ShowCursor(false);
}

void FPSCamera::Uninit()
{
	Camera::Uninit(); 
	ShowCursor(true);
}

void FPSCamera::Draw()
{
	Camera::Draw();
}

void FPSCamera::Update()
{
	Camera::Update();

	ToggleDebugMode();
	if (!m_inDebugMode)
	{
		MouseLook();
		Movement();
		Shoot();
	}
}

void FPSCamera::SetViewMatrix()
{
	// convert float4x4 to matrices
	dx::XMMATRIX view = dx::XMLoadFloat4x4(&m_mView);
	dx::XMVECTOR eye = dx::XMLoadFloat3(&m_position);
	dx::XMVECTOR forward = dx::XMLoadFloat3(&m_forward);

	dx::XMFLOAT3 fup = dx::XMFLOAT3(0, 1, 0);
	dx::XMVECTOR up = dx::XMLoadFloat3(&fup);

	// calculate and set the view matrix for each shader
	view = dx::XMMatrixLookAtLH(eye, dx::XMVectorAdd(eye, forward), up);

	auto shaders = CRenderer::GetShaders();
	for (auto shader : shaders)
	{
		shader->SetViewMatrix(&view);
	}

	// load the view matrix back to member variable
	dx::XMStoreFloat4x4(&m_mView, view);
}

void FPSCamera::MouseLook()
{
	// get cursor diff
	GetCursorPos(&m_cursorPos);
	POINT diffPoint = m_cursorPos - m_cursorFixedPos;
	
	// inverse xDeg for x rotation if looking into -z
	float xRotYDeg = diffPoint.y;
	if (m_forward.z < 0) xRotYDeg *= -1;

	// inverse xDeg for z rotation if looking into +x
	float zRotYDeg = diffPoint.y;
	if (m_forward.x > 0) zRotYDeg *= -1;

	// get rotation angle
	dx::XMMATRIX xRot, yRot, zRot;

	xRot = dx::XMMatrixRotationX(dx::XMConvertToRadians(xRotYDeg) * (1 - fabsf(m_forward.x)));
	yRot = dx::XMMatrixRotationY(dx::XMConvertToRadians((float)diffPoint.x));
	zRot = dx::XMMatrixRotationZ(dx::XMConvertToRadians(zRotYDeg) * fabsf(m_forward.x));

	// rotate forward vector
	dx::XMVECTOR vecForward = dx::XMLoadFloat3(&m_forward);
	dx::XMVECTOR temp = dx::XMVector3TransformCoord(vecForward, xRot * yRot * zRot);

	dx::XMFLOAT3 tempResult;
	dx::XMStoreFloat3(&tempResult, temp);

	if (!(tempResult.y > 0.95F || tempResult.y < -0.95F))
	{
		m_forward = tempResult;

		// calculate right vector from the new forward vector
		dx::XMMATRIX nRot;
		nRot = dx::XMMatrixRotationY(dx::XMConvertToRadians(90));

		tempResult.y = 0;
		temp = dx::XMLoadFloat3(&tempResult);

		dx::XMVECTOR rightResult = dx::XMVector3TransformCoord(temp, nRot);
		dx::XMStoreFloat3(&m_right, rightResult);
	}

	// fixate cursor back to center
	SetCursorPos(m_cursorFixedPos.x, m_cursorFixedPos.y);
	m_cursorPos = m_cursorFixedPos;
}

void FPSCamera::Movement()
{
	// normalized wasd movement
	dx::XMVECTOR moveDirection = dx::XMVectorZero();
	if (CInput::GetKeyPress(DIK_W))
		moveDirection += m_forward;
	if (CInput::GetKeyPress(DIK_A))
		moveDirection -= m_right;
	if (CInput::GetKeyPress(DIK_S))
		moveDirection -= m_forward;
	if (CInput::GetKeyPress(DIK_D))
		moveDirection += m_right;
	if (CInput::GetKeyPress(DIK_Q))
		moveDirection += {0, 1, 0};
	if (CInput::GetKeyPress(DIK_E))
		moveDirection += {0, -1, 0};

	moveDirection = dx::XMVector3Normalize(moveDirection);
	m_position += dx::XMVectorScale(moveDirection, m_moveSpeed);
}

void FPSCamera::Shoot()
{
	if (CInput::GetMouseMiddleTrigger())
	{
		CManager::GetActiveScene()->AddGameObject<Billboard>(1)->SetPosition(0,10,0);
		auto b = CManager::GetActiveScene()->AddGameObject<Bullet>(0);
		b->SetPosition(m_position);
		b->SetDirection(m_forward);
	}
}

void FPSCamera::ToggleDebugMode()
{
	if (!m_inDebugMode && CInput::GetKeyTrigger(DIK_LCONTROL))
	{
		ShowCursor(true);
		m_inDebugMode = true;
	}
	else if(m_inDebugMode && CInput::GetKeyTrigger(DIK_LCONTROL))
	{
		ShowCursor(false);
		m_inDebugMode = false;
	}
}
