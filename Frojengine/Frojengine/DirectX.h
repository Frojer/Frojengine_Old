#pragma once

#ifndef _DIRECTX_HEADER_
#define _DIRECTX_HEADER_

#include <Windows.h>

#include <d3d11.h>
#pragma comment(lib, "d3d11")

#include <DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "DXGI")

//DX ���̴� ��� ���̺귯��.
#include "D3DCompiler.h"				// DX ���̴� �����Ϸ� ���.
#pragma comment(lib, "d3dcompiler")		// DX ���̴� �����Ϸ� ���̺귯��.  D3DCompiler.dll �ʿ�.


///////////////////////////////////////////////////////////////////////////////
//
// DirectX Ȯ�� ���̺귯�� : DirectX Extension
//
// DX Ȯ�� ���̺귯�� �� ��ƿ��Ƽ ������.
// �̰��� ����Ϸ��� ������Ʈ/�Ӽ��� �߰� ������ �ʿ��մϴ�.
// DXext/����.txt �� ����. 
//
///////////////////////////////////////////////////////////////////////////////

//< DirectX Toolkit > 
// DX ��ƿ��Ƽ �� �Ϲ�ȭ ���̺귯�� ������. 
// DX ���� �ʿ��� ���� ��� Ŭ���� �� ���񽺸� �����մϴ�.
// �ڼ��� ������ �Ʒ��� �����ϼ���.
// ���̺귯�� ���� :  DXTK/ReadMe.txt
// ���� : https://blogs.msdn.microsoft.com/chuckw/2012/03/02/directxtk/
//
#include "DirectXTK.h"


#endif