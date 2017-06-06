#pragma once

#include "Frojengine\Scene\Scene.h"
#include "Hero.h"

class MainScene : public CScene
{
public:
	Hero* m_pHero;
public:
	virtual bool Load();
	virtual void Release();
};