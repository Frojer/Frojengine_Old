
//
// DirectXTK.h : DirectX Toolkit ���
//
// DirectX Toolkit : DX11 ( Dec.8, 2016) 
// ���� Present : Chuck Walbourn @ Microsoft / MSFT (http://blogs.msdn.com/b/chuckw/)
// ���� Edit    : Kihong Kim / onlysonim@gmail.com 
// ���� Update  : 2016.12.07
//

#pragma once
#ifndef _DIRECTX_TOOL_KIT_
#define _DIRECTX_TOOL_KIT_

///////////////////////////////////////////////////////////////////////////////
//
//  DirectX Took Kit  ���̺귯�� 
//
///////////////////////////////////////////////////////////////////////////////
//
//  DX ��ƿ��Ƽ �� �Ϲ�ȭ ���̺귯�� ������. 
//  DX API ���� �ʿ��� ���� ��� Ŭ���� �� ���񽺸� �����մϴ�.
//  �ڼ��� ������ �Ʒ��� �����ϼ���.
//  ���̺귯�� ���� :  DXTK/ReadMe.txt
//  ���� : https://blogs.msdn.microsoft.com/chuckw/2012/03/02/directxtk/
// 
//  <����> DirectXTK, December 5, 2016 (DX11)
//  <��� �÷���>
//  Windows Desk Top Apps(Windows 7/8/10), XBox One, Windows Mobile (Windows Phone 8.1), Windows Store Apps ����.
//  <���� ���>
//  Math, State Objects, Texture, EffectSystem, Model(XFile) Animation, Font, Audio (XAudio �� Wave), ��..
//  <���� ����ȯ��>
//  Visual Studio 2013 (update 5), Visual Studio 2015 (update 3), Windows 7 (sp1) �̻�.
//
//
//  DirectXTK �� ���� ���ϰ� ����ϱ� ���ؼ� Project �Ӽ� �߰��� �ʿ��մϴ�.
//  ���� ������� Project �Ӽ��� ����/�߰� �Ͻʽÿ�.
//
//  1. C/C++>�Է�>�Ϲ�>�߰� ���� ���͸� : "../DXTK/Inc" �߰�
//  2. ��Ŀ>�Ϲ�>�߰� ���̺귯�� ���͸� : "../DXTK/Lib " �߰�
//  3. ��Ŀ>�Է�>�߰� ���Ӽ� : DirectXTK.lib �߰� (�Ǵ� �Ʒ��� ���� ���� ����� ��)
//  4. DirectXTK Ŭ������ ��� ���� :  �������� ������Ʈ�� �ʿ��� ����� �߰�. (�Ʒ� ����)
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG 
#pragma comment(lib, "DirectXTKd")		//32bit + ����� ����.
//#pragma comment(lib, "DirectXTK64d")	//64bit + ����� ����.
#else
#pragma comment(lib, "DirectXTK")		//32bit ����.
//#pragma comment(lib, "DiectXTK64")	//64bit ���� 
#endif

//-----------------------------------------------------------------------------
//  DirectXTK ��� ���� : �츮 ������Ʈ�� �ʿ��� �͸� �߰��ϰڽ��ϴ�.
//-----------------------------------------------------------------------------
//  DirectXTK : SimpleMath 
//  SIMD ���� ���� �ڷ���(XMVECTOR �� XMMATRIX)�� ���� ���⼺�� �ּ�ȭ�� ���� ���̺귯��. 
//  �����ϰ� �������� �������� �ټ��� �������ϸ� ����մϴ�.
//  ���� : https://github.com/Microsoft/DirectXTK/wiki/SimpleMath
// 
#include <SimpleMath.h>	
using namespace DirectX::SimpleMath;

//  DirectXTK : SpriteFont ��� ��Ʈ ��� Ŭ����
//  2D Texture / Sprite ����� ��Ʈ ��� ���̺귯��.
//  ������ ����������, CJK �迭�� ��Ʈ�� ��� �ϼ����� �����˴ϴ�.
//  ���� : DXTK/MakeSpriteFont/SpriteFont ����.txt
//  ���� : https://directxtk.codeplex.com/wikipage?title=SpriteFont
//
#include <SpriteFont.h>

#endif