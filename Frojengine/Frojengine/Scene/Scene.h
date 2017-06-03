#pragma once

#include <list>
#include "..\Object\Object.h"

using namespace std;

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

	virtual bool Load() = 0;
	virtual bool CreateObjs() = 0;
	virtual void Release() = 0;

	void Update(float deltaTime);
	void Render(LPDXDC pDXDC);

	void AddObject(CObject& obj);

protected:
	void AddDeleteList(CObject* pObj);

	void ReleaseObjs();
};