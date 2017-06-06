#include "..\Frojengine.h"

CScene::CScene()
{

}

CScene::CScene(const CScene& obj)
{

}

CScene::~CScene()
{

}



void CScene::Create(LPDEVICE pDevice, LPCWSTR name)
{
	m_Name = name;
	m_pDevice = pDevice;
	m_pDevice->GetImmediateContext(&m_pDXDC);
}



void CScene::Release()
{
	for (list<CObject*>::iterator iter = m_Objs.begin(); iter != m_Objs.end(); iter++)
	{
		if ((*iter)->GetParent() == nullptr)
		{
			(*iter)->Destroy();
		}
	}

	m_Objs.clear();

	for (int i = 0; i < m_Cameras.size(); i++)
	{
		SAFE_DELETE(m_Cameras[i]);
	}

	m_Cameras.clear();

	ReleaseObjs();
}



void CScene::Update(float deltaTime)
{
	ReleaseObjs();

	for (list<CObject*>::iterator iter = m_Objs.begin(); iter != m_Objs.end(); iter++)
	{
		if ((*iter)->GetParent() == nullptr)
		{
			(*iter)->Update(deltaTime);
		}
	}
}

void CScene::Render()
{
	for (int i = 0; i < m_Cameras.size(); i++)
	{
		if (m_Cameras[i]->m_Enable)
		{
			m_Cameras[i]->UseCamera();
			SceneRender();
			for (list<CObject*>::iterator iter = m_Objs.begin(); iter != m_Objs.end(); iter++)
			{
				(*iter)->Render();
			}
		}
	}
}


void CScene::SceneRender()
{

}


void CScene::AddObject(CObject* obj)
{
	m_Objs.push_back(obj);
}


void CScene::AddCamera(CCamera* cam)
{
	m_Cameras.push_back(cam);
}


void CScene::AddDeleteList(CObject* pObj)
{
	m_ReleaseObjs.push_back(pObj);
}


void CScene::ReleaseObjs()
{
	while (m_ReleaseObjs.size() != 0)
	{
		if (m_ReleaseObjs.front() != nullptr)
		{
			m_ReleaseObjs.front()->Release();
			delete m_ReleaseObjs.front();
			m_ReleaseObjs.front() = nullptr;
		}
		
		m_ReleaseObjs.pop_front();
	}
}