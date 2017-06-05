#include "Shader.h"

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

	LPDXDC pDXDC;
	pDevice->GetImmediateContext(&pDXDC);

	result = LoadShader(pDevice, fileName);

	if (!result)
	{
		return false;
	}



	return true;
}


void CShader::Release()
{

}


void CShader::Update()
{

}


void CShader::Apply()
{

}


void CShader::UpdateCB()
{

}


bool CShader::LoadShader(LPDEVICE pDevice, LPCWSTR fileName)
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
	hr = pDevice->CreateVertexShader(	pVSCode->GetBufferPointer(), 
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
	hr = pDevice->CreatePixelShader(	pPSCode->GetBufferPointer(), 
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


bool CShader::ShaderCompile(
						LPCWSTR fileName,		// �ҽ������̸�.
						char* entryPoint,		// �����Լ� ������.
						char* shaderModel,		// ���̴� ��.
						ID3DBlob** ppCode		// [���] �����ϵ� ���̴� �ڵ�.
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


bool CShader::CreateCB()
{

}