#include "MainScene.h"
#include "Frojengine\System\GraphicsSystem.h"

bool MainScene::Load()
{
	bool result;

	CShader* pShader = new CShader;
	pShader->Create(m_pDevice, L"fx/Demo.fx");

	CMaterial* pMat = new CMaterial;
	pMat->Create(VECTOR4(1.0f, 1.0f, 1.0f, 1.0f), VECTOR4(1.0f, 1.0f, 1.0f, 1.0f), VECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, pShader);


	m_pCamera = new CCamera;

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_pCamera->Create(m_pDevice, VECTOR3(0.0f, 5.0f, -20.0f), VECTOR3(0.0f, 1.0f, 0.0f), VECTOR3(0.0f, 0.0f, 0.0f), 45.0f, 1.0f, 100.0f, 800, 600, vp);

	m_pLight = new CLight;
	m_pLight->Create(VECTOR3(1.0f, -3.0f, 2.0f), COLOR(1.0f, 1.0f, 1.0f, 1.0f), COLOR(0.2f, 0.2f, 0.2f, 1.0f), 1000.0f);

	m_pHero = new Hero;
	m_pHero->Create(m_pDevice, L"Hero", VECTOR3(0.0f, 0.0f, 0.0f), VECTOR3(0.0f, 0.0f, 0.0f), VECTOR3(1.0f, 1.0f, 1.0f));
	m_pHero->SetLight(m_pLight);

	// �ε�޽� �ȿ� ����η� �ӽ÷� �ٲ� ���߿� ���� �ʿ�
	result = LoadMesh(m_pDevice, L"Resource/hero.obj", (CObject**)&m_pHero, pMat);

	if (!result)
	{
		return false;
	}

	CShader* pShader2 = new CShader;
	pShader2->Create(m_pDevice, L"fx/Demo.fx");

	CMaterial* pMat2 = new CMaterial;
	pMat2->Create(VECTOR4(0.0f, 1.0f, 0.0f, 1.0f), VECTOR4(0.0f, 1.0f, 0.0f, 1.0f), VECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, pShader2);


	m_pTerrain = new Terrain;
	m_pTerrain->Create(m_pDevice, L"Terrain", VECTOR3(0.0f, -1.0f, 0.0f), VECTOR3(0.0f, 0.0f, 0.0f), VECTOR3(128.0f, 1.0f, 128.0f));
	m_pTerrain->SetLight(m_pLight);

	// �ε�޽� �ȿ� ����η� �ӽ÷� �ٲ� ���߿� ���� �ʿ�
	result = LoadMesh(m_pDevice, L"Resource/Terrain.obj", (CObject**)&m_pTerrain, pMat2);

	if (!result)
	{
		return false;
	}

	m_pHero->m_pMainCamera = m_pCamera;

	m_pFont = new CFontSystem;

	result = m_pFont->Create(m_pDevice, L"../Font/����9k.sfont");

	if (!result)
	{
		return false;
	}

	return true;
}



void MainScene::SceneRender()
{


	//���� �� ��Ÿ ������ ���� ���.
	//�����Ӽ� ǥ��.
	PutFPS(0, 0);

	//���� ���.
	ShowInfo();
}




/////////////////////////////////////////////////////////////////////////////
//
void MainScene::PutFPS(int x, int y)
{
	static int oldtime = GetTickCount();
	int nowtime = GetTickCount();

	static int frmcnt = 0;
	static float fps = 0.0f;

	++frmcnt;

	int time = nowtime - oldtime;
	if (time >= 999)
	{
		oldtime = nowtime;

		fps = (float)frmcnt * 1000 / (float)time;
		frmcnt = 0;
	}

	//char msg[40];
	//sprintf(msg,"FPS:%.1f/%d", fps, frmcnt );
	//g_pFont->TextDraw(NULL, msg, -1, NULL, DT_NOCLIP, COLOR(0, 1, 0, 1));
	m_pFont->TextDraw(x, y, COLOR(0, 1, 0, 1), L"FPS:%.1f/%d", fps, frmcnt);
}






/////////////////////////////////////////////////////////////////////////////// 
//
//  ����. 
//
void MainScene::ShowInfo()
{
	static bool bShow = true;
	if (IsKeyUp(VK_F1)) bShow ^= true;

	if (!bShow)
	{
		//ynTextDraw(1,20, COLOR(1, 1, 0, 1), "[Help] F1"); 
		return;
	}


	// Today's Topic.
	{
		int x = 300, y = 50;
		COLOR col(1, 1, 1, 1);
		COLOR col2(1, 1, 0, 1);
		m_pFont->TextDraw(x, y, col, L"�� %s", L"Frojengine");

		y += 24;
		WCHAR* msg =
			L"1.�⺻�����ӿ�ũ ����.\n"
			L"2.HW ������ ����̽�(DX11 Device) �� ����.\n"
			L"3.Idle �ð� ������.\n"
			L"4.����ü�� Swap(Flipping) chain �� ���� \n";
		//L"5.��üȭ�� �Ǵ� â��� ��ȯ (Alt-Enter) \n"
		//L"6.��������ȭ(VSync): Ƽ�(Tearing), ���͸�(Shuttering) ����";
		m_pFont->TextDraw(x, y, col, msg);
	}


	int x = 300, y = 300;
	static int cnt = 0;
	COLOR col(1, 1, 0, 1);
	m_pFont->TextDraw(x, y, col, L"Hello, Device!!    cnt=%08d", ++cnt);
}