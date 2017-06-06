#include "..\Frojengine.h"

list<CMesh*> CMesh::deleteList;

CMesh::CMesh()
{
	deleteList.push_back(this);

	m_Name = L"Default";
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