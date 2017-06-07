#include "..\Frojengine.h"
#include "..\System\GraphicsSystem.h"
CShader::CShader()
{
	m_pVS = nullptr;
	m_pPS = nullptr;
	m_pVSCode = nullptr;
	m_pCB_WVP = nullptr;
	m_pCB_Lit = nullptr;
	m_pCB_Mtrl = nullptr;
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
	ZeroMemory(&m_cbWVP, sizeof(CB_WVP));

	// ��� ���� ����
	CreateDynamicConstantBuffer(sizeof(CB_WVP), &m_cbWVP, &m_pCB_WVP);

	// ����� ������� ����.��
	// ���������� �ǽð����� ������ �ʴ´ٸ�, �Ϲ�(����) ������۷ε� ����ϴ�.
	ZeroMemory(&m_cbLit, sizeof(CB_LIGHT));
	CreateDynamicConstantBuffer(sizeof(CB_LIGHT), &m_cbLit, &m_pCB_Lit);

	// ������ ������� ����.��
	// ���������� �ǽð����� ������ �ʴ´ٸ�, �Ϲ�(����) ������۷ε� ����ϴ�.
	ZeroMemory(&m_cbMtrl, sizeof(CB_MATERIAL));
	CreateDynamicConstantBuffer(sizeof(CB_MATERIAL), &m_cbMtrl, &m_pCB_Mtrl);



	return true;
}


void CShader::Release()
{
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pVSCode);
	SAFE_RELEASE(m_pCB_WVP);
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
	m_pDXDC->VSSetConstantBuffers(0, 1, &m_pCB_WVP);
	m_pDXDC->VSSetConstantBuffers(1, 1, &m_pCB_Lit);
	m_pDXDC->VSSetConstantBuffers(2, 1, &m_pCB_Mtrl);
	
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


void CShader::UpdateCB(MATRIXA* pWorld, CLight* light, VECTOR diffuse, VECTOR ambient)
{
	// �ܺ� ���� ��ķ� ������� ����. 
	

	//-----------------------
	// ������� ����
	//-----------------------
	m_cbWVP.mWorld = *pWorld;
	m_cbWVP.mWV = m_cbWVP.mWorld * m_cbWVP.mView;	//��
	//m_CBuffer.mTM = XMMatrixTranspose(mTM);	//���̴����� '�� �켱 Column major' �������� ó���ϸ� �ӵ� ����� ����� �� �ֽ��ϴ�. �̸� ���� ��� ��ġ ó��.	
	m_cbWVP.mView = XMLoadFloat4x4(&CGraphicsSystem::mView);
	m_cbWVP.mProj = XMLoadFloat4x4(&CGraphicsSystem::mProj);
	//���̴� ��� ���� ����.(��������)
	UpdateDynamicConstantBuffer(m_pCB_WVP, &m_cbWVP, sizeof(CB_WVP));
	//���̴� ��� ���� ����.(��������)
	//m_pDXDC->UpdateSubresource(m_pCB, 0, nullptr, &m_CBuffer, 0, 0);

	VECTOR dir = XMLoadFloat3(&light->m_Direction);
	m_cbLit.Direction = XMVector3Normalize(-dir);
	m_cbLit.Diffuse = XMLoadFloat4(&light->m_Diffuse);
	m_cbLit.Ambient = XMLoadFloat4(&light->m_Ambient);
	m_cbLit.Range = light->m_Range;
	m_cbLit.LitOn = TRUE;
	UpdateDynamicConstantBuffer(m_pCB_Lit, &m_cbLit, sizeof(CB_LIGHT));

	m_cbMtrl.Diffuse = diffuse;
	m_cbMtrl.Ambient = ambient;
	UpdateDynamicConstantBuffer(m_pCB_Mtrl, &m_cbMtrl, sizeof(CB_MATERIAL));
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