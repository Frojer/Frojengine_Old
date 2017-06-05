#pragma once

#include <vector>

#include "Scene.h"

using namespace std;

class CSceneManager
{
private:
	static CScene* s_CurrentScene;
	static CScene* s_UpdateScene;

	static bool s_bLoading;
	static vector<CScene*> s_Scenes;

public:
	explicit CSceneManager();
	CSceneManager(const CSceneManager& obj);
	~CSceneManager();
	
	static bool LoadScene(LPCWSTR sceneName);
	static bool LoadScene(UINT sceneNumber);

	static void ChangeScene();

	static void AddScene(CScene& scene);
};