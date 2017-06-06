#pragma once

#include <list>

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

	CModel*	m_pModel;

	CObject* m_pParent;
	list<CObject*> m_Children;

protected:
	LPDEVICE m_pDevice;
	LPDXDC m_pDXDC;

public:
	explicit CObject();
	CObject(const CObject& obj);
	virtual ~CObject();

	virtual bool Create(LPDEVICE pDevice, LPCWSTR name, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale, CModel* pModel = nullptr, CObject* parent = nullptr);
	virtual void Destroy();

protected:
	virtual void Release();

public:
	virtual void Update(float deltaTime);
	virtual void Render();

	void SetParent(CObject* parent);
	void AddChild(CObject* child);

	CObject* GetParent();
	CObject* GetChild(LPCWSTR childName);

	friend void CScene::ReleaseObjs();
};

bool LoadMesh(LPDEVICE pDevice, LPCWSTR fileName, CObject* o_pObject);