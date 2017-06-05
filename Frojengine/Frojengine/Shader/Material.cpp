#include "Material.h"

CMaterial::CMaterial()
{
	m_Diffuse = VECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Ambient = VECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Specular = VECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

	m_Power = 1.0f;

	m_pShader = nullptr;
}

CMaterial::CMaterial(const CMaterial& obj)
{

}

CMaterial::~CMaterial()
{

}


bool CMaterial::Create(VECTOR4 diffuse, VECTOR4 ambient, VECTOR4 specular, float power, CShader* pShader)
{
	m_Diffuse = diffuse;
	m_Ambient = ambient;
	m_Specular = specular;
	m_Power = power;
	m_pShader = pShader;

	return true;
}


void CMaterial::Release()
{
	SAFE_RELEASE(m_pShader);
}



void CMaterial::Update()
{
	m_pShader->Update();
}



void CMaterial::Apply()
{
	m_pShader->Apply();
}