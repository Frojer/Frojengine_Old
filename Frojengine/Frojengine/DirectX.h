#pragma once

#ifndef _DIRECTX_HEADER_
#define _DIRECTX_HEADER_

#include <Windows.h>

#include <d3d11.h>
#pragma comment(lib, "d3d11")

#include <DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "DXGI")

//DX 셰이더 운용 라이브러리.
#include "D3DCompiler.h"				// DX 셰이더 컴파일러 헤더.
#pragma comment(lib, "d3dcompiler")		// DX 셰이더 컴파일러 라이브러리.  D3DCompiler.dll 필요.


///////////////////////////////////////////////////////////////////////////////
//
// DirectX 확장 라이브러리 : DirectX Extension
//
// DX 확장 라이브러리 및 유틸리티 모음집.
// 이것을 사용하려면 프로젝트/속성에 추가 설정이 필요합니다.
// DXext/사용법.txt 을 참조. 
//
///////////////////////////////////////////////////////////////////////////////

//< DirectX Toolkit > 
// DX 유틸리티 및 일반화 라이브러리 모음집. 
// DX 사용시 필요한 여러 기능 클래스 및 서비스를 제공합니다.
// 자세한 내용을 아래를 참조하세요.
// 라이브러리 설명 :  DXTK/ReadMe.txt
// 참조 : https://blogs.msdn.microsoft.com/chuckw/2012/03/02/directxtk/
//
#include "DirectXTK.h"


#endif