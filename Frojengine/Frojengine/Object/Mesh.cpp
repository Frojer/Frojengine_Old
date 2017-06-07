#include "..\Frojengine.h"

list<CMesh*> CMesh::deleteList;

CMesh::CMesh()
{
	m_Name = L"Default";
	m_Ref = 0;
}

CMesh::CMesh(const CMesh& obj)
{

}

CMesh::~CMesh()
{

}


void CMesh::Release()
{
	--m_Ref;

	if (m_Ref == 0)
	{
		deleteList.push_back(this);
	}
}



void CMesh::CheckDeleteList()
{
	while (deleteList.size() != 0)
	{
		delete deleteList.back();
		deleteList.back() = nullptr;
		deleteList.pop_back();
	}
}