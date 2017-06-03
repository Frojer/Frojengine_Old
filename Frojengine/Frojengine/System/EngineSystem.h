#pragma once
#ifndef _ENGINE_SYSTEM_
#define _ENGINE_SYSTEM_

#include "..\Frojengine.h"
#include "GraphicsSystem.h"
#include "..\Scene\SceneManager.h"

class CEngineSystem
{
public:
	static bool m_bEnd;

private:
	HWND m_hWnd;

	LPCWSTR m_ClassName;
	LPCWSTR m_WindowName;

	CGraphicsSystem* m_pGraphics;

	CScene* m_CurScene;
	CScene* m_ChangeScene;

	float m_deltaTime;

	int m_Width;
	int m_Height;

public:
	CEngineSystem();
	CEngineSystem(const CEngineSystem& obj);
	~CEngineSystem();

	bool Create(LPCWSTR className, LPCWSTR windowName, int width, int height);
	void Release();

	void Run();

	void GetWindowSize(int& oWidth, int& oHeignt);

	void PrintText(int x, int y, LPCWSTR string, int size);

private:
	bool InitWindow();
	void ResizeWindow();

	bool MessagePump();

	void GetEngineTime();

	void Update();
	void Render();

	static LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif