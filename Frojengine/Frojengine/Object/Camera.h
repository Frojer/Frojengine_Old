#pragma once

class CCamera
{
protected:
	LPDEVICE m_pDevice;
	LPDXDC m_pDXDC;

	D3D11_VIEWPORT m_Viewport;

	float m_FOV;
	float m_Aspect;

public:
	VECTOR3 m_Pos;
	VECTOR3 m_Up;
	VECTOR3 m_LookAt;

	float m_Znear;
	float m_Zfar;

	UINT m_Width;
	UINT m_Height;

	bool m_Enable;
	
public:
	CCamera();
	CCamera(const CCamera& obj);
	~CCamera();

	bool Create(LPDEVICE pDevice, VECTOR3 pos, VECTOR3 up, VECTOR3 lookAt, float fovAngle, float zNear, float zFar, UINT width, UINT height, D3D11_VIEWPORT& viewport);

	void SetViewport(D3D11_VIEWPORT& viewport);

	void UseCamera();
	void CameraUpdate();

	MATRIXA GetViewMatrixLH();
	MATRIXA GetPerspectiveFovLH(DISPLAYMODE mode);

	void SetFOV(float fovAngle) { m_FOV = XMConvertToRadians(fovAngle); }
	float GetFOV() { return m_FOV; }

	float GetAspect();
};