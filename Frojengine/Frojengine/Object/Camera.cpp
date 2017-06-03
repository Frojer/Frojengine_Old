#include "Camera.h"

CCamera::CCamera()
{

}

CCamera::CCamera(const CCamera& obj)
{

}

CCamera::~CCamera()
{

}

bool CCamera::Create(VECTOR3 pos, VECTOR3 up, VECTOR3 lookAt, float fov, float zNear, float zFar, DISPLAYMODE* pMode)
{
	m_Pos = pos;
	m_Up = up;
	m_LookAt = lookAt;

	m_FOV = fov;
	m_Znear = zNear;
	m_Zfar = zFar;
	
	m_Aspect = pMode->Width / (float)pMode->Height;

	return true;
}



void CCamera::CameraUpdate()
{

}



MATRIXA CCamera::GetViewMatrixLH()
{
	VECTOR pos = XMLoadFloat3(&m_Pos);
	VECTOR lookAt = XMLoadFloat3(&m_LookAt);
	VECTOR up = XMLoadFloat3(&m_Up);

	return XMMatrixLookAtLH(pos, lookAt, up);
}


MATRIXA CCamera::GetPerspectiveFovLH(DISPLAYMODE mode)
{
	return XMMatrixPerspectiveFovLH(m_FOV, m_Aspect, m_Znear, m_Zfar);
}


float CCamera::GetAspect()
{
	return m_Aspect;
}