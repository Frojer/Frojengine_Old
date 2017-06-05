#include "Mesh.h"

CMesh::CMesh()
{
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

	if (m_Ref == 0)
	{
		
	}
}