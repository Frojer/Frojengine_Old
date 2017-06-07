#pragma once
#ifndef _LIGHT_H_
#define _LIGHT_H_

class CLight
{
public:
	VECTOR3 m_Direction;	// ºûÀÇ ¹æÇâ
	COLOR m_Diffuse;		// ÁÖ ±¤·® : È®»ê±¤ Diffuse Light
	COLOR m_Ambient;		// º¸Á¶ ±¤·® : ÁÖº¯±¤ Ambient Light
	FLOAT m_Range;			// ºû µµ´Þ °Å¸®

public:
	CLight();
	CLight(const CLight& obj);
	~CLight();

	bool Create(VECTOR3 direction, COLOR diffuse, COLOR ambient, FLOAT range);
	void Release();
};

#endif