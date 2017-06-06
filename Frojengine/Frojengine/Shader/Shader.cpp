#include "..\Frojengine.h"

CShader::CShader()
{
	m_pVS = nullptr;
	m_pPS = nullptr;
	m_pVSCode = nullptr;
	m_pCB = nullptr;
}

CShader::CShader(const CShader& obj)
{

}

CShader::~CShader()
{

}


bool CShader::Create(LPDEVICE pDevice, LPCWSTR fileName)
{
	bool result;

	m_pDevice = pDevice;
	m_pDevice->GetImmediateContext(&m_pDXDC);

	result = LoadShader(fileName);

	if (!result)
	{
		return false;
	}

	// ���� �Է±��� ����
	//LoadLayout();

	// ��� ������ �ʱ�ȭ
	ZeroMemory(&m_CBuffer, sizeof(CB_WVP));

	// ��� ���� ����
	CreateDynamicConstantBuffer(sizeof(CB_WVP), &m_CBuffer, &m_pCB);



	return true;
}


void CShader::Release()
{
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pVSCode);
	SAFE_RELEASE(m_pCB);
}


void CShader::Update()
{

}


void CShader::Apply()
{
	//------------------------------------
	// ������ �ɼ� : �ܺο��� ����.
	//------------------------------------ 
	//... �ʿ�� �߰� ...

	//------------------------------------
	// �޽� -����̽��� ��� : �ܺο��� ����.
	//------------------------------------
	// ���� ���� ���� : �ܺο��� ���� ... 
	//...
	// �Է� ���̾ƿ� ����. 
	// m_pDXDC->IASetInputLayout(m_pLayout);
	// ���� ���� ���� ���� : �ܺο��� ���� ... 
	//...

	//------------------------------------
	// ���̴� ����.
	//------------------------------------
	//��� ���� ���� & ����.
	//...
	m_pDXDC->VSSetConstantBuffers(0, 1, &m_pCB);
	
	//���̴� ����.
	m_pDXDC->VSSetShader(m_pVS, nullptr, 0);
	m_pDXDC->PSSetShader(m_pPS, nullptr, 0);
	

	//------------------------------------
	// �׸���! 
	//------------------------------------
	// ... �ܺο��� ���� ... 
	 

	//------------------------------------
	// ������ �ɼ� ���� : Ÿ ����Ʈ/���� �������� ���� ��ġ ���� ����(�ɼ�)
	//------------------------------------
	//... �ܺο��� ���� / �ʿ�� �߰� ...
}


void CShader::UpdateCB(MATRIXA* pWorld)
{
	// �ܺ� ���� ��ķ� ������� ����. 
	

	//-----------------------
	// ������� ����
	//-----------------------
	XMStoreFloat4x4(&m_CBuffer.mWorld, *pWorld);
	//m_CBuffer.mTM = XMMatrixTranspose(mTM);	//���̴����� '�� �켱 Column major' �������� ó���ϸ� �ӵ� ����� ����� �� �ֽ��ϴ�. �̸� ���� ��� ��ġ ó��.	

	//���̴� ��� ���� ����.(��������)
	UpdateDynamicConstantBuffer(m_pCB, &m_CBuffer, sizeof(CB_WVP));
	//���̴� ��� ���� ����.(��������)
	//m_pDXDC->UpdateSubresource(m_pCB, 0, nullptr, &m_CBuffer, 0, 0);
}




////////////////////////////////////////////////////////////////////////////// 
//
// ���̴� �б� : ���̴��� Ÿ ����Ʈ�� ������ �����ϵ��� ������ ���谡 �ʿ��մϴ�.
//
// �� ����Ʈ�� ���� ���̴��� �����ϰų� ��ü �������� �����Ǵ� ���̴��� ���� �� �ֽ��ϴ�.
// ���̴��� �ʿ�� '��ü����'�� ������� ����սô�.
//
bool CShader::LoadShader(LPCWSTR fileName)
{
	HRESULT hr = S_OK;
	
		
	//-------------------------
	// ���� ���̴� �ε�.
	//-------------------------
	LPVERTEXSHADER pVS = nullptr;
	ID3DBlob* pVSCode = nullptr;
	hr = ShaderCompile(fileName, "VS_Main", "vs_5_0", &pVSCode);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"[����] ShaderLoad :: Vertex Shader ������ ����", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// ���� ���̴� ��ü ���� Create the vertex shader
	hr = m_pDevice->CreateVertexShader(	pVSCode->GetBufferPointer(), 
									    pVSCode->GetBufferSize(), 
									    nullptr, 
	  								    &pVS	    
			                        );
	if (FAILED(hr))
	{
		SAFE_RELEASE(pVSCode);			//�ӽ� ��ü ����.
		return false;
	}

	//���� ���� : : Input Layout ���۽� �ʿ���.
	//m_pVSCode = pVSCode;

		
	
	//-------------------------
	// �ȼ� ���̴� �ε�.
	//-------------------------
	LPPIXELSHADER pPS = nullptr;
	ID3DBlob* pPSCode = nullptr;
	hr = ShaderCompile(fileName, "PS_Main", "ps_5_0", &pPSCode);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"[����] ShaderLoad :: Pixel Shader ������ ����", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// �ȼ� ���̴� ��ü ���� Create the pixel shader
	hr = m_pDevice->CreatePixelShader(	pPSCode->GetBufferPointer(), 
										pPSCode->GetBufferSize(), 
										nullptr,
										&pPS
									);


	SAFE_RELEASE(pPSCode);				//�ӽ� ��ü ����.	


	//----------------
	// ����Ʈ ���� ����.(�Ϸ�)
	//----------------
	m_pVS = pVS;
	m_pPS = pPS;
	m_pVSCode = pVSCode;
	

	return true;
}



////////////////////////////////////////////////////////////////////////////// 
//
// ���̴� �ҽ� ������ : ���̴� �ҽ�(*.fx)�� GPU �� ����� ��ȯ�մϴ�. 
//
bool CShader::ShaderCompile(
						LPCWSTR fileName,		// �ҽ������̸�.
						char* entryPoint,		// �����Լ� ������.
						char* shaderModel,		// ���̴� ��.
						LPXDATA* ppCode		// [���] �����ϵ� ���̴� �ڵ�.
						)
{
	HRESULT hr = S_OK;
	ID3DBlob* pError = nullptr;

	//������ �ɼ�1.
	UINT Flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;		//���켱 ��� ó��. ���� DX9 ���������� �������� ���. �ӵ��� �䱸�ȴٸ�, "��켱" ���� ó���� ��.
														//UINT Flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;	//��켱 ��� ó��. �� �켱 ó������ �ӵ��� ����� ������, ����� ��ġ�Ѽ� GPU �� �����ؾ� �Ѵ�.
														//UINT Flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
	Flags |= D3DCOMPILE_DEBUG;							//����� ���� �ɼ� �߰�.
#endif

														//���̴� �ҽ� ������.
	hr = D3DCompileFromFile(fileName,
		nullptr, nullptr,
		entryPoint,
		shaderModel,
		Flags,				//������ �ɼ�.1
		0,					//������ �ɼ�2,  Effect ���� �����Ͻ� �����. �̿ܿ��� ���õ�.
		ppCode,				//[���] �����ϵ� ���̴� �ڵ�.
		&pError				//[���] ������ ���� �ڵ�.
	);
	if (FAILED(hr))
	{
		WindowErrorW(TRUE, L"Effect::���̴� ������ ����", hr, pError, fileName, entryPoint, shaderModel);
		return false;
	}

	SAFE_RELEASE(pError);
	return true;
}



////////////////////////////////////////////////////////////////////////////// 
//
// (����) ��� ���� ����
//
bool CShader::CreateConstantBuffer(UINT size, LPBUFFER* ppCB)
{
	HRESULT hr;
	LPBUFFER pCB = nullptr;

	// ��� ���� ���� ����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	
	// ��� ���� ����
	hr = m_pDevice->CreateBuffer(&bd, nullptr, &pCB);

	if (FAILED(hr))
	{
		WindowError(hr, L"Shader::CreateConstantBuffer : ������� ���� ����");
		return false;
	}

	*ppCB = pCB;

	return true;
}




////////////////////////////////////////////////////////////////////////////// 
//
// (����) ��� ���� ����
//
bool CShader::CreateDynamicConstantBuffer(UINT size, LPVOID pData, LPBUFFER* ppCB)
{
	HRESULT hr = S_OK;
	LPBUFFER pCB = nullptr;

	// ��� ���� ���� ����.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;					// ���� �������� ����.
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// CPU ���� ����.


	// ���긮�ҽ� ����.
	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = pData;				// (�ܺ�) ��� ������ ����.
	sd.SysMemPitch = 0;
	sd.SysMemSlicePitch = 0;

	// ��� ���� ����.
	hr = m_pDevice->CreateBuffer(&bd, &sd, &pCB);
	if (FAILED(hr))
	{
		WindowError(hr, L"Shader::CreateDynamicConstantBuffer : ���� ������� ���� ����");
		return false;
	}

	// �ܺη� ����.
	*ppCB = pCB;

	return true;
}




////////////////////////////////////////////////////////////////////////////// 
//
// ���� ���� ����.
//
bool CShader::UpdateDynamicConstantBuffer(ID3D11Resource* pBuff, LPVOID pData, UINT size)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mr;
	ZeroMemory(&mr, sizeof(mr));

	// ������� ����
	hr = m_pDXDC->Map(pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
	if (FAILED(hr))
	{
		WindowError(hr, L"Effect::UpdateDynamicConstantBuffer : Map ����");
		return false;
	}

	// ��� ���� ����.
	memcpy(mr.pData, pData, size);

	// ������� �ݱ�.
	m_pDXDC->Unmap(pBuff, 0);


	return true;
}