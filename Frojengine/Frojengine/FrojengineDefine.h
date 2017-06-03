#pragma once

#ifndef _FROJER_DEFINE_
#define _FROJER_DEFINE_

///////////////////////////////////////////////////////////////////////////////
//
// 기타 시스템 관련 함수 및 재정의.
//
///////////////////////////////////////////////////////////////////////////////

//비동기 키보드 입력 처리.
#define IsKeyDown(key)	((GetAsyncKeyState(key)&0x8000) == 0x8000)
#define IsKeyUp(key)	((GetAsyncKeyState(key)&0x8001) == 0x8001)

#define SAFE_RELEASE(pObj)	if ((pObj) != nullptr) { (pObj)->Release(); (pObj) = NULL; }
#define SAFE_DELETE(pObj)	if ((pObj) != nullptr) { delete (pObj); (pObj) = NULL; }
#define SAFE_DELARRY(pObj)	if ((pObj) != nullptr) { delete [] (pObj); (pObj) = NULL; }

void WindowErrorW(TCHAR* file, UINT line, TCHAR* func, BOOL bMBox, HRESULT hr, TCHAR* msg, ...);

#define WindowError(hr, msg, ...)	\
WindowErrorW(__FILEW__, __LINE__, __FUNCTIONW__, TRUE, hr, msg, __VA_ARGS__)

#endif