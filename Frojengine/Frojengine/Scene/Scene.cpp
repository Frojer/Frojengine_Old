#include "Scene.h"

CScene::CScene()
{

}

CScene::CScene(const CScene& obj)
{

}

CScene::~CScene()
{

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

void CScene::Render(LPDXDC pDXDC)
{
	for (list<CObject*>::iterator iter = m_Objs.begin(); iter != m_Objs.end(); iter++)
	{
		(*iter)->Render(pDXDC);
	}
}


void CScene::AddObject(CObject& obj)
{
	m_Objs.push_back(&obj);
}


void CScene::AddDeleteList(CObject* pObj)
{
	m_ReleaseObjs.push_back(pObj);
}


void CScene::ReleaseObjs()
{
	while (m_ReleaseObjs.size() != 0)
	{
		m_ReleaseObjs.front()->Release();
		delete m_ReleaseObjs.front();
		m_ReleaseObjs.front() = nullptr;
		m_ReleaseObjs.pop_front();
	}
}