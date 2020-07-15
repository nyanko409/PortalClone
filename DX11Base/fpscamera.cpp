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
}

void FPSCamera::Uninit()
{
	Camera::Uninit(); 
}

void FPSCamera::Draw()
{
	Camera::Draw();
}

void FPSCamera::Update()
{
	Camera::Update();
	MouseLook();
	Movement();
	Shoot();
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
	D3DXMATRIX xRot, yRot, zRot;

	D3DXMatrixRotationX(&xRot, D3DXToRadian(xRotYDeg) * (1 - fabsf(m_forward.x)));
	D3DXMatrixRotationY(&yRot, D3DXToRadian((float)diffPoint.x));
	D3DXMatrixRotationZ(&zRot, D3DXToRadian(zRotYDeg) * fabsf(m_forward.x));

	// rotate forward vector
	D3DXVECTOR3 temp;
	D3DXVec3TransformCoord(&temp, &m_forward, &(xRot * yRot * zRot));

	if (!(temp.y > 0.95F || temp.y < -0.95F))
	{
		m_forward = temp;

		// calculate right vector from the new forward vector
		D3DXMATRIX nRot;
		D3DXMatrixRotationY(&nRot, D3DXToRadian(90));
		D3DXVec3TransformCoord(&m_right, &(D3DXVECTOR3(m_forward.x, 0, m_forward.z)), &(nRot));
	}

	// fixate cursor back to center
	SetCursorPos(m_cursorFixedPos.x, m_cursorFixedPos.y);
	m_cursorPos = m_cursorFixedPos;
}

void FPSCamera::Movement()
{
	// normalized wasd movement
	D3DXVECTOR3 moveDirection = { 0,0,0 };
	if (CInput::GetKeyPress('W'))
		moveDirection += m_forward;
	if (CInput::GetKeyPress('A'))
		moveDirection -= m_right;
	if (CInput::GetKeyPress('S'))
		moveDirection -= m_forward;
	if (CInput::GetKeyPress('D'))
		moveDirection += m_right;
	if (CInput::GetKeyPress('Q'))
		moveDirection += {0, 1, 0};
	if (CInput::GetKeyPress('E'))
		moveDirection += {0, -1, 0};

	D3DXVec3Normalize(&moveDirection, &moveDirection);
	m_position += moveDirection * m_moveSpeed;
}

void FPSCamera::Shoot()
{
	if (CInput::GetKeyTrigger(VK_SPACE))
	{		
		Bullet* b = CManager::GetActiveScene()->AddGameObject<Bullet>(0);
		b->SetPosition(m_position);
		b->SetDirection(m_forward);
	}
}
