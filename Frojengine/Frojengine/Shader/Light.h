#pragma once
#ifndef _LIGHT_H_
#define _LIGHT_H_

class CLight
{
public:
	VECTOR3 m_Direction;	// ���� ����
	COLOR m_Diffuse;		// �� ���� : Ȯ�걤 Diffuse Light
	COLOR m_Ambient;		// ���� ���� : �ֺ��� Ambient Light
	FLOAT m_Range;			// �� ���� �Ÿ�

public:
	CLight();
	CLight(const CLight& obj);
	~CLight();

	bool Create(VECTOR3 direction, COLOR diffuse, COLOR ambient, FLOAT range);
	void Release();
};

#endif