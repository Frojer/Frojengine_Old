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

	// ��� ���ۿ� ����ü : ���̴� ���� ���꿡 ���� �����͵�. 
	// GPU �� �����͸� ������ ����� �����͸� �����մϴ�.  
	struct CB_WVP
	{
		MATRIXA mWorld;			//"World" ��ȯ ��� ���� : DirectXMath, ������, SIMD �񰡼�, �Ϲ� �����.
		MATRIXA mView;		//"View" ��ȯ ��� ���� 
		MATRIXA mWV;		//"World * View" ��ȯ ���.
		MATRIXA mProj;		//"Projection" ��ȯ ��� ����

	};

	//���� ������ ������� ����ü
	struct CB_LIGHT
	{
		XMVECTOR Direction;		//���� ����.
		XMVECTOR Diffuse;		//�� ���� : Ȯ�걤 Diffuse Light.
		XMVECTOR Ambient;		//���� ���� : �ֺ��� Ambient Light.
		FLOAT    Range;			//�� ���� �Ÿ�.
		BOOL	 LitOn;			//���� ���뿩��.
	};

	//���� ������ ������� ����ü
	struct CB_MATERIAL
	{
		XMVECTOR Diffuse;		//�� ����(Ȯ�걤) �� �ݻ���(%) 
		XMVECTOR Ambient;		//���� ����(�ֺ���) �� �ݻ���(%) 
	};

	//���̴� ���޿� ���������. 
	//�� ���ֺ� �����͸� ���̴��� ������.
	CB_WVP m_cbWVP;
	CB_LIGHT m_cbLit;
	CB_MATERIAL m_cbMtrl;

protected:
	bool LoadShader(LPCWSTR fileName);
	bool ShaderCompile(LPCWSTR fileName, char* entryPoint, char* shaderModel, LPXDATA* ppCode);

	// (����) ��� ���� ����
	bool CreateConstantBuffer(UINT size, LPBUFFER* ppCB);

	// (����) ��� ���� ����
	bool CreateDynamicConstantBuffer(UINT size, LPVOID pData, LPBUFFER* ppCB);

	// ���� ���� ����
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

	// ��� ������.
	LPVSCODE  GetVSCode() { return m_pVSCode; }
	LPVOID	  GetBufferPointer() { return m_pVSCode->GetBufferPointer(); }
	SIZE_T	  GetBufferSize() { return m_pVSCode->GetBufferSize(); }
	CB_WVP* GetCBuffer() { return &m_cbWVP; }
};

#endif