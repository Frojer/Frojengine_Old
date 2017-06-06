#include "MainScene.h"

bool MainScene::Load()
{
	m_pHero = new Hero;

	/*m_pHero->Create(AddDeleteList, L"Hero", VECTOR3(0.0f, 0.0f, 0.0f), VECTOR3(0.0f, 0.0f, 0.0f), VECTOR3(1.0f, 1.0f, 1.0f));*/

	return true;
}