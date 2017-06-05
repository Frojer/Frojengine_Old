#pragma once
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Shader.h"

class CMaterial
{
public:
	VECTOR4		m_Diffuse;
	VECTOR4		m_Ambient;
	VECTOR4		m_Specular;
	float		m_Power;

	CShader*	m_pShader;

public:
	CMaterial();
	CMaterial(const CMaterial& obj);
	~CMaterial();

	bool Create();
	void Release();
};

#endif