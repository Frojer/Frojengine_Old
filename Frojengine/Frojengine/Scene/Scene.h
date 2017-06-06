#pragma once

#include <list>

using namespace std;

class CObject;

class CScene	
{
protected:
	LPDEVICE m_pDevice;
	LPDXDC m_pDXDC;

	list<CObject*> m_Objs;
	list<CObject*> m_ReleaseObjs;
	vector<CCamera*> m_Cameras;

public:
	LPCWSTR m_Name;

	COLOR m_bkColor;

private:
	void Release();

	void Update(float deltaTime);
	void Render();

public:
	explicit CScene();
	CScene(const CScene& obj);
	~CScene();

	void Create(LPDEVICE pDevice, LPCWSTR name);
	virtual bool Load() = 0;
	virtual void SceneRender();

	void AddObject(CObject* obj);
	void AddCamera(CCamera* cam);
	void AddDeleteList(CObject* pObj);
	void ReleaseObjs();

	friend void CSceneManager::ChangeScene();
	friend void CSceneManager::Release();
	friend class CEngineSystem;
};