#pragma once
#ifndef _GRAPHICS_SYSTEM_
#define _GRAPHICS_SYSTEM_

#include "..\Frojengine.h"


// 레스터 상태객체
enum {
	RS_DEFAULT,				// 기본 렌더링 : 솔리드 + Culling off.
	RS_WIREFRM,				// 와이어프레임 렌더링.
	RS_CULLBACK,			// 뒷면 컬링 On! "CCW"
	RS_WIRECULLBACK,		// 와이어 프레임 + 컬링 On! "CCW"

	RS_MAX_
};


// 깊이/스텐실 버퍼 상태객체.
enum {
	SO_DEPTH_ON,			// 깊이버퍼 ON! (기본값)
	SO_DEPTH_OFF,			// 깊이버퍼 OFF!
	SO_DEPTH_WRITE_OFF,		// 깊이버퍼 쓰기 끄기.

	SO_MAX_,
};



// 렌더링 모드 : 1가지 이상의 렌더링 모드 조합 가능.
enum {
	RM_DEFAULT = 0x0000,		// 기본모드 : Solid + Cull-Off.
	RM_SOLID = RM_DEFAULT,	// 기본모드
	RM_WIREFRAME = 0x0001,		// 와이어 프레임 
	RM_CULLBACK = 0x0002,		// 뒷면 컬링 "CCW"
};


class CGraphicsSystem
{
private:
	LPDEVICE m_pDevice;
	LPDXDC m_pDXDC;					// 장치 상태 및 렌더링 명령 관리 인터페이스
	LPSWAPCHAIN m_pSwapChain;
	LPRTVIEW m_pRTView;

	// 깊이 스텐실 버퍼 관련.
	ID3D11Texture2D*		 m_pDS;			// 깊이-스텐실 버퍼.
	ID3D11DepthStencilView*  m_pDSView;		// 깊이-스텐실 뷰.

	DISPLAYMODE m_Mode;

	// 레스터 상태객체.
	ID3D11RasterizerState* m_RState[RS_MAX_];

	// 깊이/스텐실 버퍼 상태객체.
	ID3D11DepthStencilState* m_DSState[SO_MAX_];

	DWORD m_RMode;

public:
	static COLOR m_BackColor;

	static MATRIX mView;
	static MATRIX mProj;

	static bool m_bWindowMode;
	static bool m_bVSync;
	// 뒷면 제거...On/Off.
	static bool m_bCullBack;
	// 와이어 프레임 On/Off.
	static bool m_bWireFrame;
	// 깊이 버퍼 연산 On/Off.
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


	// Get, Set 함수들
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