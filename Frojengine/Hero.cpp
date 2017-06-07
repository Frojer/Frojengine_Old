#include "Hero.h"

void Hero::Update(float deltaTime)
{
	// 이동 값 계산. : 이동 행렬 생성. 
	//
	float mov = 5.0f * deltaTime;
	if (IsKeyDown(VK_LEFT))	m_Pos.x -= mov;
	if (IsKeyDown(VK_RIGHT))	m_Pos.x += mov;
	if (IsKeyDown(VK_UP))	m_Pos.z += mov;
	if (IsKeyDown(VK_DOWN))	m_Pos.z -= mov;
	// 이동행렬 만들기.	
	CObject::Update(deltaTime);
}


void Hero::Render()
{
	CObject::Render();
}