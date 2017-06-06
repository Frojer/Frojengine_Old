#include "MainScene.h"

bool MainScene::Load(LPDEVICE pDevice)
{
	LoadMesh(pDevice, L"", m_pHero);

	return true;
}



void MainScene::Release()
{
	m_pHero->Destroy();
}