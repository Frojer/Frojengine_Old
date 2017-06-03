#pragma once

#include <vector>

#include "..\Frojengine.h"

using namespace std;

struct VertexFormat
{
	float x, y, z;
	float r, g, b, a;

	VertexFormat(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _r = 1.0f, float _g = 1.0f, float _b = 1.0f, float _a = 1.0f)
	{
		x = _x;
		y = _y;
		z = _z;
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}
};

class CMesh
{
public:
	vector<VertexFormat>	m_VF;
	
	LPVERTEXBUFFER	m_pVB;
	LPINDEXBUFFER	m_pIB;
	LPINPUTLAYOUT	m_pInputLayout;

public:
	CMesh();
	CMesh(const CMesh& obj);
	~CMesh();

	bool Create(LPDEVICE device, LPCWSTR fileName);
	void Release();

	void Draw(LPDXDC pDXDC);

private:
	bool LoadMesh(LPCWSTR fileName);
	bool CreateBuffer(LPDEVICE device);
};