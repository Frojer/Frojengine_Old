#include "Hero.h"

void Hero::Update(float deltaTime)
{
	m_Speed = 5.0f;


	//ȸ�� ó��.  
	float rot = XM_PI * 0.5f * deltaTime;		//90��/sec �� ȸ��.(����)
	if (IsKeyDown('A'))  m_Rot.y += rot;
	if (IsKeyDown('D'))  m_Rot.y -= rot;
	if (IsKeyDown('W'))  m_Rot.x += rot;
	if (IsKeyDown('S'))  m_Rot.x -= rot;

	// �̵� �� ���. : �̵� ��� ����.
	float mov = m_Speed * deltaTime;
	if (IsKeyDown(VK_LEFT))	m_Pos.x -= mov;
	if (IsKeyDown(VK_RIGHT))	m_Pos.x += mov;
	if (IsKeyDown(VK_UP))	m_Pos.z += mov;
	if (IsKeyDown(VK_DOWN))	m_Pos.z -= mov;

	m_pMainCamera->m_LookAt = m_Pos;

	// �̵���� �����.	
	CObject::Update(deltaTime);
}


void Hero::Render()
{
	CObject::Render();
}