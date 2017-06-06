#include "..\Frojengine.h"

CCamera::CCamera()
{

}

CCamera::CCamera(const CCamera& obj)
{

}

CCamera::~CCamera()
{

}

bool CCamera::Create(LPDEVICE pDevice, VECTOR3 pos, VECTOR3 up, VECTOR3 lookAt, float fovAngle, float zNear, float zFar, UINT width, UINT height, D3D11_VIEWPORT& viewport)
{
	m_pDevice = pDevice;
	m_pDevice->GetImmediateContext(&m_pDXDC);

	m_Pos = pos;
	m_Up = up;
	m_LookAt = lookAt;

	m_FOV = XMConvertToRadians(fovAngle);
	m_Znear = zNear;
	m_Zfar = zFar;
	
	m_Width = width;
	m_Height = height;

	m_Aspect = (FLOAT)m_Width / (FLOAT)m_Height;

	m_Viewport = viewport;

	CSceneManager::CurrentScene->AddCamera(this);

	m_Enable = true;

	return true;
}



void CCamera::SetViewport(D3D11_VIEWPORT& viewport)
{
	m_Viewport = viewport;
}



void CCamera::UseCamera()
{
	m_pDXDC->RSSetViewports(1, &m_Viewport);
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