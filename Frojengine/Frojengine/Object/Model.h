#pragma once

#include "..\Frojengine.h"
#include "Mesh.h"
#include "..\Shader\Material.h"

using namespace std;

class CModel
{
protected:
	LPDEVICE	m_pDevice;
	LPDXDC		m_pDXDC;

protected:
	CMesh*			m_pMesh;
	
	LPVERTEXBUFFER	m_pVB;
	LPINDEXBUFFER	m_pIB;
	LPINPUTLAYOUT	m_pInputLayout;

	UINT			m_Stride;		// 정점 크기.
	UINT			m_Offset;		// 렌더링 시작 정점 (offset)

	CMaterial*		m_pMaterial;

protected:
	bool LoadMaterial(CMaterial* pMaterial);
	bool LoadLayout();
	bool CreateBuffer();

public:
	explicit CModel();
	CModel(const CModel& obj);
	virtual ~CModel();

	virtual bool Create(LPDEVICE pDevice, CMesh* mesh, CMaterial* pMaterial);
	virtual void Release();

	virtual void Update(float deltaTime);
	virtual void Draw();

	virtual void UpdateCB(MATRIXA* mTM);
};