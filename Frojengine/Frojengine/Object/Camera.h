#pragma once

#include "..\Frojengine.h"

class CCamera
{
public:
	VECTOR3 m_Pos;
	VECTOR3 m_Up;
	VECTOR3 m_LookAt;

	float m_FOV;
	float m_Znear;
	float m_Zfar;
		
private:
	float m_Aspect;

public:
	CCamera();
	CCamera(const CCamera& obj);
	~CCamera();

	bool Create(VECTOR3 pos, VECTOR3 up, VECTOR3 lookAt, float fov, float zNear, float zFar, DISPLAYMODE* pMode);

	void CameraUpdate();

	MATRIXA GetViewMatrixLH();
	MATRIXA GetPerspectiveFovLH(DISPLAYMODE mode);

	float GetAspect();
};