#pragma once

#ifndef _FROJER_DEFINE_
#define _FROJER_DEFINE_


///////////////////////////////////////////////////////////////////////////////
//
// Ÿ�� �� ��� ������ : ������ ȣȯ�� ��� ���밡 ����.
//
///////////////////////////////////////////////////////////////////////////////
//DX11 ��ġ���� ������.											
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
typedef ID3DBlob*			LPXDATA;			// ���� ������ ����� ����.(Vertex, Adjacency, material, binary code, etc..)
typedef LPXDATA				LPVSCODE;			// �����ϵ� ���̴� ���̳ʸ��ڵ� ���� ����.


/*
// DirectX Math Ÿ�� ������ : ȣȯ�� ���
// DX9/DX10 : DirectX SDK June.2010 ����
typedef D3DXMATRIXA16	MATRIXA;	//��� : 16����Ʈ ���� ����.
typedef D3DXMATRIX		MATRIX;		//��� : �Ϲ� ����.
typedef D3DXVECTOR4		VECTOR4;
typedef D3DXVECTOR3		VECTOR3;
typedef D3DXVECTOR2		VECTOR2;
*/
// DirectX Math Ÿ�� ������ : ���� �ҽ����� ȣȯ�� ����� ����.
// DX10/11 "XNAMath ver.2": DirectX SDK june.2010
// DX11/12 "XNAMath ver.3": DirectXMath, Windows SDK 8.x �� ����  
// ��ũ : https://msdn.microsoft.com/ko-kr/library/windows/desktop/ee418728(v=vs.85).aspx
//
typedef XMMATRIX		MATRIXA;	//��� : 16����Ʈ ����, SIMD ����. ����/���� ������. "Register Type"
typedef XMFLOAT4X4		MATRIX;		//��� : �Ϲ� ����. SIMD ������, Class ������ �����. "Storage Type"
typedef XMVECTOR		VECTOR;		//4���� ���� : 16����Ʈ ����, SIMD ����. ����/���� ������. "Register Type"
typedef XMFLOAT4		VECTOR4;	//4���� ���� : �Ϲ� ����, SIMD ������, Class ������ �����. "Storage Type"
typedef XMFLOAT3		VECTOR3;
typedef XMFLOAT2		VECTOR2;

//���� Ÿ��: 2����.
//typedef XMCOLOR		COLOR;		// r, g, b, a.  [������ 0~255]
typedef XMFLOAT4		COLOR;		// r, g, b, a.  [�Ǽ��� 0~1.0]

// ��ġ ���� ���� ����ü. (DX9/11 ���� ȣȯ�� ������)
typedef DXGI_MODE_DESC	  DISPLAYMODE;	//DX11 ����
//typedef DXGI_MODE_DESC1 DISPLAYMODE;	//DX11.1 ����
//typedef D3DDISPLAYMODE DISPLAYMODE;   //DX9 ��.




///////////////////////////////////////////////////////////////////////////////
//
// ��Ÿ �ý��� ���� �Լ� �� ������.
//
///////////////////////////////////////////////////////////////////////////////

//�񵿱� Ű���� �Է� ó��.
#define IsKeyDown(key)	((GetAsyncKeyState(key)&0x8000) == 0x8000)
#define IsKeyUp(key)	((GetAsyncKeyState(key)&0x8001) == 0x8001)

#define SAFE_RELEASE(pObj)	if ((pObj) != nullptr) { (pObj)->Release(); (pObj) = NULL; }
#define SAFE_DELETE(pObj)	if ((pObj) != nullptr) { delete (pObj); (pObj) = NULL; }
#define SAFE_DELARRY(pObj)	if ((pObj) != nullptr) { delete [] (pObj); (pObj) = NULL; }

void WindowErrorW(TCHAR* file, UINT line, TCHAR* func, BOOL bMBox, HRESULT hr, TCHAR* msg, ...);
void WindowErrorW(BOOL bMBox, TCHAR* msg, HRESULT hr, ID3DBlob* pBlob, TCHAR* filename, char* EntryPoint, char* ShaderModel);

#define WindowError(hr, msg, ...)	\
WindowErrorW(__FILEW__, __LINE__, __FUNCTIONW__, TRUE, hr, msg, __VA_ARGS__)

#endif