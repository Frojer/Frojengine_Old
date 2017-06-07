#pragma once
#ifndef _LINE_H_
#define _LINE_H_

struct LineVertexFormat
{
	VECTOR3 position;
	VECTOR4 color;

	LineVertexFormat(VECTOR3 position = { 0.0f, 0.0f, 0.0f },
		VECTOR4 color = { 1.0f, 1.0f, 1.0f, 1.0f })
	{
		this->position = position;
		this->color = color;
	}
};

class CLine
{
protected:
	LPDEVICE m_pDevice;
	LPDXDC m_pDXDC;

	static CShader* m_pShader;

public:
	LineVertexFormat m_startPos;
	LineVertexFormat m_endPos;

	LPVERTEXBUFFER m_pVB;
	LPINPUTLAYOUT m_pInputLayout;

public:
	explicit CLine();
	CLine(const CLine& obj);
	virtual ~CLine();

	bool Create(LPDEVICE pDevice, const LineVertexFormat& startPos, const LineVertexFormat& endPos);
	void Release();
};

#endif