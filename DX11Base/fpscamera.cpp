#include "pch.h"
#include "main.h"
#include "fpscamera.h"
#include "input.h"
#include "renderer.h"
#include "shader.h"
#include "player.h"


POINT operator - (POINT o1, POINT o2) { return POINT{ o1.x - o2.x, o1.y - o2.y }; }

void FPSCamera::Init()
{
	Camera::Init();

	m_moveSpeed = 0.2f;
	m_height = 3.0f;
	m_mouseLook = true;

	ScreenToClient(GetWindow(), &m_cursorFixedPos);
	m_cursorFixedPos.x = fabsf(m_cursorFixedPos.x) + SCREEN_WIDTH / 2.0F;
	m_cursorFixedPos.y = fabsf(m_cursorFixedPos.y) + SCREEN_HEIGHT / 2.0F;

	m_forward = dx::XMFLOAT3(0, 0, 1);
	m_right = dx::XMFLOAT3(1, 0, 0);
	SetPosition(0, m_height, 0 );

	while (ShowCursor(false) >= 0);
}

void FPSCamera::Uninit()
{
	Camera::Uninit(); 
	
	while (ShowCursor(true) < 0);
}

void FPSCamera::Draw(Pass pass)
{
	Camera::Draw(pass);
}

void FPSCamera::Update()
{
	Camera::Update();

	ToggleDebugMode();
	if (!m_inDebugMode && m_mouseLook)
	{
		MouseLook();
	}
}

dx::XMMATRIX FPSCamera::GetLocalToWorldMatrix(bool ignoreXZRotation) const
{
	dx::XMVECTOR eyePos = GetPosition();
	dx::XMFLOAT4X4 t = {};
	dx::XMVECTOR forward = GetForwardVector();
	if (ignoreXZRotation)
		forward = dx::XMVectorSetY(forward, 0);

	dx::XMVECTOR zaxis = dx::XMVector3Normalize(forward);
	dx::XMVECTOR xaxis = dx::XMVector3Normalize(GetRightVector());
	dx::XMVECTOR yaxis = dx::XMVector3Cross(zaxis, xaxis);

	dx::XMFLOAT3 z, x, y;
	dx::XMStoreFloat3(&z, zaxis);
	dx::XMStoreFloat3(&x, xaxis);
	dx::XMStoreFloat3(&y, yaxis);

	t._11 = x.x;
	t._12 = x.y;
	t._13 = x.z;
	t._21 = y.x;
	t._22 = y.y;
	t._23 = y.z;
	t._31 = z.x;
	t._32 = z.y;
	t._33 = z.z;
	t._41 = dx::XMVectorGetX(eyePos);
	t._42 = dx::XMVectorGetY(eyePos);
	t._43 = dx::XMVectorGetZ(eyePos);
	t._44 = 1;

	return dx::XMLoadFloat4x4(&t);
}

void FPSCamera::SetViewMatrix()
{
	// convert float4x4 to matrices
	dx::XMMATRIX view = dx::XMLoadFloat4x4(&m_mView);
	dx::XMVECTOR forward = dx::XMLoadFloat3(&m_forward);
	dx::XMVECTOR up = dx::XMVectorSet(0, 1, 0, 0);
	dx::XMVECTOR eye = GetPosition();

	if (auto target = m_target.lock())
	{
		up = dx::XMLoadFloat3(&target->virtualUp);
	}

	// calculate and set the view matrix for each shader
	view = dx::XMMatrixLookToLH(eye, forward, up);

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
	dx::XMVECTOR temp = dx::XMVector3Transform(vecForward, xRot * yRot * zRot);

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

		dx::XMVECTOR rightResult = dx::XMVector3Transform(temp, nRot);
		dx::XMStoreFloat3(&m_right, rightResult);
	}

	// fixate cursor back to center
	SetCursorPos(m_cursorFixedPos.x, m_cursorFixedPos.y);
	m_cursorPos = m_cursorFixedPos;
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

		// reposition the cursor to center to prevent moving the cursor unintentionally after exiting debug mode
		SetCursorPos(m_cursorFixedPos.x, m_cursorFixedPos.y);
		m_cursorPos = m_cursorFixedPos;
	}
}
