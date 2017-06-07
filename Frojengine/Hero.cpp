#include "Hero.h"

void Hero::Update(float deltaTime)
{
	m_Speed = 5.0f;


	//회전 처리.  
	float rot = XM_PI * 0.5f * deltaTime;		//90º/sec 씩 회전.(라디안)
	if (IsKeyDown('A'))  m_Rot.y += rot;
	if (IsKeyDown('D'))  m_Rot.y -= rot;
	if (IsKeyDown('W'))  m_Rot.x += rot;
	if (IsKeyDown('S'))  m_Rot.x -= rot;

	// 이동 값 계산. : 이동 행렬 생성.
	float mov = m_Speed * deltaTime;
	if (IsKeyDown(VK_LEFT))	m_Pos.x -= mov;
	if (IsKeyDown(VK_RIGHT))	m_Pos.x += mov;
	if (IsKeyDown(VK_UP))	m_Pos.z += mov;
	if (IsKeyDown(VK_DOWN))	m_Pos.z -= mov;

	m_pMainCamera->m_LookAt = m_Pos;

	// 이동행렬 만들기.	
	CObject::Update(deltaTime);
}


void Hero::Render()
{
	CObject::Render();
}