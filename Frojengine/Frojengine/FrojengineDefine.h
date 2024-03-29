#pragma once

#ifndef _FROJER_DEFINE_
#define _FROJER_DEFINE_


///////////////////////////////////////////////////////////////////////////////
//
// 타입 과 상수 재정의 : 버전별 호환성 향상 증대가 목적.
//
///////////////////////////////////////////////////////////////////////////////
//DX11 장치관련 재정의.											
typedef ID3D11Device*			LPDEVICE;
typedef ID3D11DeviceContext*	LPDXDC;
typedef IDXGISwapChain*			LPSWAPCHAIN;
typedef ID3D11RenderTargetView*	LPRTVIEW;


typedef ID3D11Buffer*		LPBUFFER;
typedef LPBUFFER			LPVERTEXBUFFER;
typedef LPBUFFER			LPINDEXBUFFER;
typedef ID3D11InputLayout*	LPINPUTLAYOUT;

typedef ID3D11Texture2D*	LPTEXTURE2D;

typedef ID3D11VertexShader*	LPVERTEXSHADER;
typedef ID3D11PixelShader*	LPPIXELSHADER;
typedef ID3DBlob*			LPXDATA;			// 임의 데이터 저장용 버퍼.(Vertex, Adjacency, material, binary code, etc..)
typedef LPXDATA				LPVSCODE;			// 컴파일된 셰이더 바이너리코드 저장 버퍼.


/*
// DirectX Math 타입 재정의 : 호환성 향상
// DX9/DX10 : DirectX SDK June.2010 이하
typedef D3DXMATRIXA16	MATRIXA;	//행렬 : 16바이트 정렬 버전.
typedef D3DXMATRIX		MATRIX;		//행렬 : 일반 버전.
typedef D3DXVECTOR4		VECTOR4;
typedef D3DXVECTOR3		VECTOR3;
typedef D3DXVECTOR2		VECTOR2;
*/
// DirectX Math 타입 재정의 : 구형 소스와의 호환성 향상이 목적.
// DX10/11 "XNAMath ver.2": DirectX SDK june.2010
// DX11/12 "XNAMath ver.3": DirectXMath, Windows SDK 8.x 에 포함  
// 링크 : https://msdn.microsoft.com/ko-kr/library/windows/desktop/ee418728(v=vs.85).aspx
//
typedef XMMATRIX		MATRIXA;	//행렬 : 16바이트 정렬, SIMD 버전. 전역/지역 변수용. "Register Type"
typedef XMFLOAT4X4		MATRIX;		//행렬 : 일반 버전. SIMD 미지원, Class 데이터 저장용. "Storage Type"
typedef XMVECTOR		VECTOR;		//4성분 벡터 : 16바이트 정렬, SIMD 버전. 전역/지역 변수용. "Register Type"
typedef XMFLOAT4		VECTOR4;	//4성분 벡터 : 일반 버전, SIMD 미지원, Class 데이터 저장용. "Storage Type"
typedef XMFLOAT3		VECTOR3;
typedef XMFLOAT2		VECTOR2;

//색상 타입: 2가지.
//typedef XMCOLOR		COLOR;		// r, g, b, a.  [정수형 0~255]
typedef XMFLOAT4		COLOR;		// r, g, b, a.  [실수형 0~1.0]

// 장치 설정 정보 구조체. (DX9/11 구형 호환성 유지용)
typedef DXGI_MODE_DESC	  DISPLAYMODE;	//DX11 대응
//typedef DXGI_MODE_DESC1 DISPLAYMODE;	//DX11.1 대응
//typedef D3DDISPLAYMODE DISPLAYMODE;   //DX9 용.




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
void WindowErrorW(BOOL bMBox, TCHAR* msg, HRESULT hr, ID3DBlob* pBlob, TCHAR* filename, char* EntryPoint, char* ShaderModel);
void WindowErrorW(BOOL bMBox, TCHAR* msg, ...);

#define WindowError(hr, msg, ...)	\
WindowErrorW(__FILEW__, __LINE__, __FUNCTIONW__, TRUE, hr, msg, __VA_ARGS__)

#endif