#pragma once
#ifndef _SHADER_H_
#define _SHADER_H_

class CShader
{
protected:
	LPDEVICE		m_pDevice;
	LPDXDC			m_pDXDC;

	LPVERTEXSHADER	m_pVS;
	LPPIXELSHADER	m_pPS;
	LPVSCODE		m_pVSCode;
	LPBUFFER		m_pCB_WVP;
	LPBUFFER		m_pCB_Lit;
	LPBUFFER		m_pCB_Mtrl;

	// 상수 버퍼용 구조체 : 셰이더 내부 연산에 사용될 데이터들. 
	// GPU 로 데이터를 전송할 사용자 데이터를 정의합니다.  
	struct CB_WVP
	{
		MATRIXA mWorld;			//"World" 변환 행렬 변수 : DirectXMath, 비정렬, SIMD 비가속, 일반 저장용.
		MATRIXA mView;		//"View" 변환 행렬 변수 
		MATRIXA mWV;		//"World * View" 변환 행렬.
		MATRIXA mProj;		//"Projection" 변환 행렬 변수

	};

	//조명 정보용 상수버퍼 구조체
	struct CB_LIGHT
	{
		XMVECTOR Direction;		//빛의 방향.
		XMVECTOR Diffuse;		//주 광량 : 확산광 Diffuse Light.
		XMVECTOR Ambient;		//보조 광량 : 주변광 Ambient Light.
		FLOAT    Range;			//빛 도달 거리.
		BOOL	 LitOn;			//조명 적용여부.
	};

	//재질 정보용 상수버퍼 구조체
	struct CB_MATERIAL
	{
		XMVECTOR Diffuse;		//주 광량(확산광) 의 반사율(%) 
		XMVECTOR Ambient;		//보조 광량(주변광) 의 반사율(%) 
	};

	//셰이더 공급용 상수데이터. 
	//각 유닛별 데이터를 셰이더로 전달함.
	CB_WVP m_cbWVP;
	CB_LIGHT m_cbLit;
	CB_MATERIAL m_cbMtrl;

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

	virtual void UpdateCB(MATRIXA* pWorld, CLight* light, VECTOR diffuse, VECTOR ambient);

	// 멤버 접근자.
	LPVSCODE  GetVSCode() { return m_pVSCode; }
	LPVOID	  GetBufferPointer() { return m_pVSCode->GetBufferPointer(); }
	SIZE_T	  GetBufferSize() { return m_pVSCode->GetBufferSize(); }
	CB_WVP* GetCBuffer() { return &m_cbWVP; }
};

#endif