#pragma once

#include <vector>

using namespace std;

class CScene;

class CSceneManager
{
private:
	CScene* m_ChangeScene;

	bool m_bLoading;
	vector<CScene*> m_Scenes;

public:
	static CScene* CurrentScene;

public:
	explicit CSceneManager();
	CSceneManager(const CSceneManager& obj);
	~CSceneManager();

	void Release();
	
	bool LoadScene(LPCWSTR sceneName);
	bool LoadScene(UINT sceneNumber);

	void ChangeScene();

	void AddScene(CScene* scene);
};