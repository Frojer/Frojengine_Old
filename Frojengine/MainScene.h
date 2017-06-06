#pragma once

#include "Frojengine\Frojengine.h"
#include "Frojengine\System\FontSystem.h"
#include "Hero.h"

class MainScene : public CScene
{
public:
	CCamera* m_pCamera;
	CFontSystem* m_pFont;
	Hero* m_pHero;
public:
	virtual bool Load();

	virtual void SceneRender();

	void PutFPS(int x, int y);
	void ShowInfo();
};