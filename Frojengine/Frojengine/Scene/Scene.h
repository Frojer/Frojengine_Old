#pragma once

#include <list>

using namespace std;

class CObject;

class CScene	
{
public:
	LPCWSTR m_Name;
	list<CObject*> m_Objs;
	list<CObject*> m_ReleaseObjs;

	COLOR m_bkColor;

public:
	explicit CScene();
	CScene(const CScene& obj);
	~CScene();

	virtual bool Load(LPDEVICE pDevice) = 0;
	virtual void Release() = 0;

	void Update(float deltaTime);
	void Render();

	void AddObject(CObject* obj);
	void AddDeleteList(CObject* pObj);
	void ReleaseObjs();
};