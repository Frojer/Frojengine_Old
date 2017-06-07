#pragma once

#include <vector>
#include <fstream>
#include <unordered_map>

using namespace std;

struct VertexFormat
{
	VECTOR3	position;
	VECTOR3	normal;
	VECTOR2	texture;

	VertexFormat(VECTOR3 position = { 0.0f,  0.0f, 0.0f },
		VECTOR3 normal = { 0.0f,  0.0f, 0.0f },
		VECTOR2 texture = { 0.0f,  0.0f })
	{
		this->position = position;
		this->normal = normal;
		this->texture = texture;
	}
};

class CMesh
{
private:
	UINT	m_Ref;

	static list<CMesh*> deleteList;
public:
	wstring					m_Name;

	vector<VertexFormat>	m_Vertices;
	vector<int>				m_Indices;

public:
	explicit CMesh();
	CMesh(const CMesh& obj);
	virtual ~CMesh();

	void Release();

	void AddReference() { ++m_Ref; }

	void SetMeshName(wstring name) { m_Name = name; }
	UINT GetVertexSize() { return m_Vertices.size(); }
	size_t GetIndexSize() { return m_Indices.size(); }

	static void CheckDeleteList();
};