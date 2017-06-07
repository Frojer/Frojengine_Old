#include "Hero.h"

void Hero::Update(float deltaTime)
{
	// �̵� �� ���. : �̵� ��� ����. 
	//
	float mov = 5.0f * deltaTime;
	if (IsKeyDown(VK_LEFT))	m_Pos.x -= mov;
	if (IsKeyDown(VK_RIGHT))	m_Pos.x += mov;
	if (IsKeyDown(VK_UP))	m_Pos.z += mov;
	if (IsKeyDown(VK_DOWN))	m_Pos.z -= mov;
	// �̵���� �����.	
	CObject::Update(deltaTime);
}


void Hero::Render()
{
	CObject::Render();
}