#pragma once
#ifndef _SHADER_H_
#define _SHADER_H_

#include "..\Frojengine.h"

class CShader
{
protected:
	LPDEVICE		m_pDevice;
	LPDXDC			m_pDXDC;

	LPVERTEXSHADER	m_pVS;
	LPPIXELSHADER	m_pPS;
	LPVSCODE		m_pVSCode;
	LPBUFFER		m_pCB;

	// 상수 버퍼용 구조체 : 셰이더 내부 연산에 사용될 데이터들. 
	// GPU 로 데이터를 전송할 사용자 데이터를 정의합니다.  
	struct ConstBufferData
	{
		XMFLOAT4X4 mTM;			//"World" 변환 행렬 변수 : DirectXMath, 비정렬, SIMD 비가속, 일반 저장용.
		XMFLOAT4X4 mView;		//"View" 변환 행렬 변수 
		XMFLOAT4X4 mProj;		//"Projection" 변환 행렬 변수

	};

	//셰이더 공급용 상수데이터. 
	//각 유닛별 데이터를 셰이더로 전달함.
	ConstBufferData m_CBuffer;

protected:
	bool LoadShader(LPCWSTR fileName);
	bool ShaderCompile(LPCWSTR fileName, char* entryPoint, char* shaderModel, LPXDATA* ppCode);

	// (정적) 상수 버퍼 생성
	bool CreateConstantBuffer(UINT size, LPBUFFER* ppCB);

	// (종적) 상수 버퍼 생성
	bool CreateDynamicConstantBuffer(UINT size, LPVOID pData, LPBUFFER* ppCB);

	// 동적 버퍼 갱신
	bool UpdateDynamicConstantBuffer(ID3D11Resource* pBuff, LPVOID pData, UINT size);

public:
	explicit CShader();
	CShader(const CShader& obj);
	virtual ~CShader();

	virtual bool Create(LPDEVICE pDevice, LPCWSTR fileName);
	virtual void Release();

	virtual void Update();
	virtual void Apply();

	virtual void UpdateCB(MATRIXA* pTM);

	// 멤버 접근자.
	LPVSCODE  GetVSCode() { return m_pVSCode; }
	LPVOID	  GetBufferPointer() { return m_pVSCode->GetBufferPointer(); }
	UINT	  GetBufferSize() { return m_pVSCode->GetBufferSize(); }
	ConstBufferData* GetCBuffer() { return &m_CBuffer; }
};

#endif