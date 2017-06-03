///////////////////////
//
//  Object.cpp
//
///////////////////////

#include "Object.h"

CObject::CObject()
{
	m_Name = nullptr;

	m_Pos = VECTOR3(0.0f, 0.0f, 0.0f);
	m_Rot = VECTOR3(0.0f, 0.0f, 0.0f);
	m_Scale = VECTOR3(1.0f, 1.0f, 1.0f);

	m_Look = VECTOR3(0.0f, 1.0f, 0.0f);

	XMStoreFloat4x4(&m_PosM, XMMatrixIdentity());
	XMStoreFloat4x4(&m_RotM, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ScaleM, XMMatrixIdentity());
	XMStoreFloat4x4(&m_WorldM, XMMatrixIdentity());

	m_pMesh = nullptr;

	m_pParent = nullptr;
	m_Children.clear();
}

CObject::CObject(const CObject& obj)
{

}

CObject::~CObject()
{

}


bool CObject::Create(void(*AddDeleteList)(CObject*), LPCWSTR name, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale, CMesh* pMesh, CObject* parent)
{
	this->AddDeleteList = AddDeleteList;

	m_Name = name;
	m_Pos = pos;
	m_Rot = rot;
	m_Scale = scale;

	m_pMesh = pMesh;
	SetParent(parent);

	return true;
}


void CObject::Destroy()
{
	AddDeleteList(this);
}


void CObject::Release()
{
	SetParent(nullptr);

	list<CObject*>::iterator temp;
	for (list<CObject*>::iterator iter = m_Children.begin(); iter != m_Children.end();)
	{
		temp = iter;
		++iter;

		(*temp)->Release();
		delete (*temp);
		(*iter) = nullptr;
	}
}

void CObject::Update(float deltaTime)
{
	MATRIXA mPos;
	mPos = XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);

	MATRIXA mRot;
	mRot = XMMatrixRotationRollPitchYaw(m_Rot.x, m_Rot.y, m_Rot.z);

	MATRIXA mScale;
	mScale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

	MATRIXA mWorld = mScale * mRot * mPos;

	VECTOR look = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	look = XMVector3TransformCoord(look, mRot);
	XMStoreFloat3(&m_Look, look);

	if (m_pParent != nullptr)
	{
		mWorld *= XMLoadFloat4x4(&m_pParent->m_WorldM);
	}

	XMStoreFloat4x4(&m_PosM, mPos);
	XMStoreFloat4x4(&m_RotM, mRot);
	XMStoreFloat4x4(&m_ScaleM, mScale);
	XMStoreFloat4x4(&m_WorldM, mWorld);

	for (list<CObject*>::iterator iter = m_Children.begin(); iter != m_Children.end(); iter++)
	{
		(*iter)->Update(deltaTime);
	}
}



void CObject::Render(LPDXDC pDXDC)
{

}



void CObject::SetParent(CObject* parent)
{
	if (m_pParent != nullptr)
	{
		m_pParent->m_Children.remove(this);
		m_pParent = nullptr;
	}

	m_pParent = parent;
}

void CObject::AddChild(CObject* child)
{
	m_Children.push_back(child);
}

CObject* CObject::GetParent()
{
	return m_pParent;
}

CObject* CObject::GetChild(LPCWSTR childName)
{
	for (list<CObject*>::iterator iter = m_Children.begin(); iter != m_Children.end(); iter++)
	{
		if (wcscmp((*iter)->m_Name, childName) == 0)
		{
			return (*iter);
		}
	}

	return nullptr;
}