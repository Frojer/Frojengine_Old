#pragma once

#include <list>

#include "..\Frojengine.h"
#include "Mesh.h"

using namespace std;

class CObject
{
public:
	LPCWSTR m_Name;

	VECTOR3 m_Pos;
	VECTOR3 m_Rot;
	VECTOR3 m_Scale;

protected:
	VECTOR3 m_Look;

	MATRIX m_PosM;
	MATRIX m_RotM;
	MATRIX m_ScaleM;
	MATRIX m_WorldM;

	CMesh*	m_pMesh;

	CObject* m_pParent;
	list<CObject*> m_Children;

	void (*AddDeleteList)(CObject*);

public:
	explicit CObject();
	CObject(const CObject& obj);
	virtual ~CObject();

	virtual bool Create(void(*AddDeleteList)(CObject*), LPCWSTR name, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale, CMesh* pMesh = nullptr, CObject* parent = nullptr);
	virtual void Destroy();
	virtual void Release();

	virtual void Update(float deltaTime);
	virtual void Render(LPDXDC pDXDC);

	void SetParent(CObject* parent);
	void AddChild(CObject* child);

	CObject* GetParent();
	CObject* GetChild(LPCWSTR childName);
};