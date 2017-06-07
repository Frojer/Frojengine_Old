#pragma once

#include <list>

using namespace std;

class CObject
{
public:
	wstring m_Name;

	VECTOR3 m_Pos;
	VECTOR3 m_Rot;
	VECTOR3 m_Scale;
	list<CObject*> m_Children;

protected:
	VECTOR3 m_Look;

	MATRIX m_PosM;
	MATRIX m_RotM;
	MATRIX m_ScaleM;
	MATRIX m_WorldM;

	CLight* m_pLight;

	CModel*	m_pModel;

	CObject* m_pParent;

protected:
	LPDEVICE m_pDevice;
	LPDXDC m_pDXDC;

public:
	explicit CObject();
	CObject(const CObject& obj);
	virtual ~CObject();

	virtual bool Create(LPDEVICE pDevice, wstring name, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale, CModel* pModel = nullptr, CObject* parent = nullptr);
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

	void SetModel(CModel* pModel) { SAFE_RELEASE(m_pModel); m_pModel = pModel; }
	CModel* GetModel() { return m_pModel; }

	void SetMaterial(CMaterial* pMat);

	void SetLight(CLight* pLight) { m_pLight = pLight; }

	friend void CScene::ReleaseObjs();
};

bool LoadMesh(LPDEVICE pDevice, LPCWSTR fileName, CObject** o_pObject, CMaterial* pMat);