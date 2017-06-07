#include "..\Frojengine.h"

CLight::CLight()
{

}

CLight::CLight(const CLight& obj)
{

}

CLight::~CLight()
{

}


bool CLight::Create(VECTOR3 direction, COLOR diffuse, COLOR ambient, FLOAT range)
{
	m_Direction = direction;
	m_Diffuse = diffuse;
	m_Ambient = ambient;
	m_Range = range;

	return true;
}


void CLight::Release()
{

}