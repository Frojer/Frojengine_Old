#pragma once

#include "Frojengine\Frojengine.h"

class Hero : public CObject
{
public:
	CCamera* m_pMainCamera;
	float m_Speed;
public:
	virtual void Update(float deltaTime);
	virtual void Render();
};