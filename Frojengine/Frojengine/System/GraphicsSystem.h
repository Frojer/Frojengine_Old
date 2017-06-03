#pragma once
#ifndef _GRAPHICS_SYSTEM_
#define _GRAPHICS_SYSTEM_

#include "..\Frojengine.h"

class CGraphicsSystem
{
private:
	LPDEVICE m_pDevice;
	LPDXDC m_pDXDC;					// ��ġ ���� �� ������ ��� ���� �������̽�
	LPSWAPCHAIN m_pSwapChain;
	LPRTVIEW m_pRTView;

	DISPLAYMODE m_Mode;

	COLOR m_BackColor;

	bool m_bWindowMode;
	bool m_bVSync;

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
	void SetViewPort();

	void GetDeviceInfo();
	void SystemInfo(int x, int y, COLOR col);

public:
	void ClearBackBuffer();
	void Flip();


	// Get, Set �Լ���
	void SetBackColor(COLOR backColor);
	COLOR GetBackColor();

	void SetWindowMode(bool bWindow);
	bool GetWindowMode();

	void SetVSync(bool bVSync);
	bool GetVSync();
};

#endif