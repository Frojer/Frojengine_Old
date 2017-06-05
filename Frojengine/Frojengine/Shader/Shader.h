#pragma once
#ifndef _SHADER_H_
#define _SHADER_H_

#include "..\Frojengine.h"

class CShader
{
	LPVERTEXSHADER	m_pVS;
	LPPIXELSHADER	m_pPS;
	LPVSCODE		m_pVSCode;
	LPBUFFER		m_pCB;

	// ��� ���ۿ� ����ü : ���̴� ���� ���꿡 ���� �����͵�. 
	// GPU �� �����͸� ������ ����� �����͸� �����մϴ�.  
	struct ConstBuffer
	{
		XMFLOAT4X4 mTM;			//"World" ��ȯ ��� ���� : DirectXMath, ������, SIMD �񰡼�, �Ϲ� �����.
		XMFLOAT4X4 mView;		//"View" ��ȯ ��� ���� 
		XMFLOAT4X4 mProj;		//"Projection" ��ȯ ��� ����

	};

	//���̴� ���޿� ���������. 
	//�� ���ֺ� �����͸� ���̴��� ������.
	ConstBuffer m_CBuffer;

public:
	explicit CShader();
	CShader(const CShader& obj);
	virtual ~CShader();

	virtual bool Create(LPDEVICE pDevice, LPCWSTR fileName);
	virtual void Release();

	virtual void Update();
	virtual void Apply();

	virtual void UpdateCB();

private:
	bool LoadShader(LPDEVICE pDevice, LPCWSTR fileName);
	bool ShaderCompile(LPCWSTR fileName, char* entryPoint, char* shaderModel, ID3DBlob** ppCode);
	bool CreateCB();
};

#endif