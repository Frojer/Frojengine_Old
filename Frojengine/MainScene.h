#pragma once

#include "Frojengine\Frojengine.h"
#include "Frojengine\System\FontSystem.h"
#include "Hero.h"
#include "Terrain.h"

class MainScene : public CScene
{
public:
	CCamera* m_pCamera;
	CFontSystem* m_pFont;
	CLight* m_pLight;
	Hero* m_pHero;
	Terrain* m_pTerrain;

public:
	virtual bool Load();

	virtual void SceneRender();

	void PutFPS(int x, int y);
	void ShowInfo();
};