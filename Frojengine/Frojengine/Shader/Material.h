#pragma once
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

class CMaterial
{
protected:
	VECTOR4		m_Diffuse;
	VECTOR4		m_Ambient;
	VECTOR4		m_Specular;
	float		m_Power;

	CShader*	m_pShader;

public:
	CMaterial();
	CMaterial(const CMaterial& obj);
	~CMaterial();

	bool Create(VECTOR4 diffuse, VECTOR4 ambient, VECTOR4 specular, float power, CShader* pShader);
	void Release();

	void Update();
	void Apply();

	// Get, Set ÇÔ¼ö
	CShader* GetShader() { return m_pShader; }
};

#endif