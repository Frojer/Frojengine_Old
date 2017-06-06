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
	LPBUFFER		m_pCB;

	// ��� ���ۿ� ����ü : ���̴� ���� ���꿡 ���� �����͵�. 
	// GPU �� �����͸� ������ ����� �����͸� �����մϴ�.  
	struct CB_WVP
	{
		XMFLOAT4X4 mWorld;			//"World" ��ȯ ��� ���� : DirectXMath, ������, SIMD �񰡼�, �Ϲ� �����.
		XMFLOAT4X4 mView;		//"View" ��ȯ ��� ���� 
		XMFLOAT4X4 mProj;		//"Projection" ��ȯ ��� ����

	};

	//���̴� ���޿� ���������. 
	//�� ���ֺ� �����͸� ���̴��� ������.
	CB_WVP m_CBuffer;

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

	virtual void UpdateCB(MATRIXA* pWorld);

	// ��� ������.
	LPVSCODE  GetVSCode() { return m_pVSCode; }
	LPVOID	  GetBufferPointer() { return m_pVSCode->GetBufferPointer(); }
	UINT	  GetBufferSize() { return m_pVSCode->GetBufferSize(); }
	CB_WVP* GetCBuffer() { return &m_CBuffer; }
};

#endif