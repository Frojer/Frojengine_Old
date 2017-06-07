#pragma once
#ifndef _GRAPHICS_SYSTEM_
#define _GRAPHICS_SYSTEM_

#include "..\Frojengine.h"


// ������ ���°�ü
enum {
	RS_DEFAULT,				// �⺻ ������ : �ָ��� + Culling off.
	RS_WIREFRM,				// ���̾������� ������.
	RS_CULLBACK,			// �޸� �ø� On! "CCW"
	RS_WIRECULLBACK,		// ���̾� ������ + �ø� On! "CCW"

	RS_MAX_
};


// ����/���ٽ� ���� ���°�ü.
enum {
	SO_DEPTH_ON,			// ���̹��� ON! (�⺻��)
	SO_DEPTH_OFF,			// ���̹��� OFF!
	SO_DEPTH_WRITE_OFF,		// ���̹��� ���� ����.

	SO_MAX_,
};



// ������ ��� : 1���� �̻��� ������ ��� ���� ����.
enum {
	RM_DEFAULT = 0x0000,		// �⺻��� : Solid + Cull-Off.
	RM_SOLID = RM_DEFAULT,	// �⺻���
	RM_WIREFRAME = 0x0001,		// ���̾� ������ 
	RM_CULLBACK = 0x0002,		// �޸� �ø� "CCW"
};


class CGraphicsSystem
{
private:
	LPDEVICE m_pDevice;
	LPDXDC m_pDXDC;					// ��ġ ���� �� ������ ��� ���� �������̽�
	LPSWAPCHAIN m_pSwapChain;
	LPRTVIEW m_pRTView;

	// ���� ���ٽ� ���� ����.
	ID3D11Texture2D*		 m_pDS;			// ����-���ٽ� ����.
	ID3D11DepthStencilView*  m_pDSView;		// ����-���ٽ� ��.

	DISPLAYMODE m_Mode;

	// ������ ���°�ü.
	ID3D11RasterizerState* m_RState[RS_MAX_];

	// ����/���ٽ� ���� ���°�ü.
	ID3D11DepthStencilState* m_DSState[SO_MAX_];

	DWORD m_RMode;

public:
	static COLOR m_BackColor;

	static MATRIX mView;
	static MATRIX mProj;

	static bool m_bWindowMode;
	static bool m_bVSync;
	// �޸� ����...On/Off.
	static bool m_bCullBack;
	// ���̾� ������ On/Off.
	static bool m_bWireFrame;
	// ���� ���� ���� On/Off.
	static bool m_bZEnable;
	static bool m_bZWrite;

public:
	CGraphicsSystem();
	CGraphicsSystem(const CGraphicsSystem& obj);
	~CGraphicsSystem();

	bool Create(HWND hWnd, int width, int height);
	void Release();

private:
	bool CreateDX(HWND hWnd);
	void ReleaseDX();

	bool CreateDeviceSwapChain(HWND hWnd);
	bool CreateRenderTarget();
	bool CreateDepthStencil();
	void SetViewPort();
	void StateObjectCreate();
	void StateObjectRelease();

	void RasterStateLoad();
	void RasterStateRelease();
	void DSStateLoad();
	void DSStateRelease();

	void GetDeviceInfo();
	void SystemInfo(int x, int y, COLOR col);

public:
	void ClearBackBuffer();
	void RenderModeUpdate();
	void Flip();


	// Get, Set �Լ���
	static void SetBackColor(COLOR backColor);
	static COLOR GetBackColor();

	static void SetWindowMode(bool bWindow);
	static bool GetWindowMode();

	static void SetCullBack(bool bCullBack) { m_bCullBack = bCullBack; }
	static bool GetCullBack() { return m_bCullBack; }

	static void SetWireFrame(bool bWireFrame) { m_bWireFrame = bWireFrame; }
	static bool GetWireFrame() { return m_bWireFrame; }

	static void SetZEnable(bool bZEnable) { m_bZEnable = bZEnable; }
	static bool GetZEnable() { return m_bZEnable; }

	static void SetZWrite(bool bZWrite) { m_bZWrite = bZWrite; }
	static bool GetZWrite() { return m_bZWrite; }

	static void SetVSync(bool bVSync);
	static bool GetVSync();

	static void SetViewMatrix(MATRIX _mView) { mView = _mView; }
	static void SetProjectionMatrix(MATRIX _mProj) { mProj = _mProj; }

	LPDEVICE GetDevice() { return m_pDevice; }
};

#endif