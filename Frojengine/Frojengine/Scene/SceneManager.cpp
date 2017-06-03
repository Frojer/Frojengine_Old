#include "SceneManager.h"

CScene* CSceneManager::s_CurrentScene = nullptr;
CScene* CSceneManager::s_UpdateScene = nullptr;
bool CSceneManager::s_bLoading = false;
vector<CScene*> CSceneManager::s_Scenes;

CSceneManager::CSceneManager()
{

}

CSceneManager::CSceneManager(const CSceneManager& obj)
{

}

CSceneManager::~CSceneManager()
{

}

bool CSceneManager::LoadScene(LPCWSTR sceneName)
{
	for (int i = 0; i < s_Scenes.size(); i++)
	{
		if (wcscmp(s_Scenes[i]->m_Name, sceneName) == 0)
		{
			CSceneManager::s_UpdateScene = s_Scenes[i];
			break;
		}
	}

	return true;
}

bool CSceneManager::LoadScene(int sceneNumber)
{
	if (s_Scenes.size() <= sceneNumber)
		return false;

	CSceneManager::s_UpdateScene = s_Scenes[sceneNumber];

	return true;
}

void CSceneManager::ChangeScene()
{
	if (s_bLoading || s_UpdateScene == nullptr)
		return;

	CSceneManager::s_CurrentScene = CSceneManager::s_UpdateScene;
	CSceneManager::s_UpdateScene = nullptr;
	s_bLoading = false;

	CSceneManager::s_CurrentScene->Load();
}

void CSceneManager::AddScene(CScene& scene)
{
	s_Scenes.push_back(&scene);
}