#include "..\Frojengine.h"

CScene* CSceneManager::CurrentScene = nullptr;

CSceneManager::CSceneManager()
{
	m_ChangeScene = nullptr;
	m_bLoading = false;
}

CSceneManager::CSceneManager(const CSceneManager& obj)
{

}

CSceneManager::~CSceneManager()
{

}



void CSceneManager::Release()
{
	for (UINT i = 0; i < m_Scenes.size(); i++)
	{
		m_Scenes[i]->Release();
		delete m_Scenes[i];
		m_Scenes[i] = nullptr;
	}

	m_Scenes.clear();
}



bool CSceneManager::LoadScene(LPCWSTR sceneName)
{
	for (UINT i = 0; i < m_Scenes.size(); i++)
	{
		if (wcscmp(m_Scenes[i]->m_Name, sceneName) == 0)
		{
			m_ChangeScene = m_Scenes[i];
			break;
		}
	}

	return true;
}

bool CSceneManager::LoadScene(UINT sceneNumber)
{
	if (m_Scenes.size() <= sceneNumber)
		return false;

	m_ChangeScene = m_Scenes[sceneNumber];

	return true;
}

void CSceneManager::ChangeScene(LPDEVICE pDevice)
{
	if (m_bLoading || m_ChangeScene == nullptr)
		return;

	CSceneManager::CurrentScene = CSceneManager::m_ChangeScene;
	CSceneManager::m_ChangeScene = nullptr;
	m_bLoading = false;

	CSceneManager::CurrentScene->Load(pDevice);
}

void CSceneManager::AddScene(CScene* scene)
{
	m_Scenes.push_back(scene);
}