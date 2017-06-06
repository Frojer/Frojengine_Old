#pragma once

#include "Frojengine\Frojengine.h"
#include "Hero.h"

class MainScene : public CScene
{
public:
	Hero* m_pHero;
public:
	virtual bool Load(LPDEVICE pDevice);
	virtual void Release();
};