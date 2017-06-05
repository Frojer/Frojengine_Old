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
	// 정점 셰이더 로드.
	//-------------------------
	LPVERTEXSHADER pVS = nullptr;
	ID3DBlob* pVSCode = nullptr;
	hr = ShaderCompile(fileName, "VS_Main", "vs_5_0", &pVSCode);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"[실패] ShaderLoad :: Vertex Shader 컴파일 실패", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// 정점 셰이더 객체 생성 Create the vertex shader
	hr = pDevice->CreateVertexShader(	pVSCode->GetBufferPointer(), 
									    pVSCode->GetBufferSize(), 
									    nullptr, 
	  								    &pVS	    
			                        );
	if (FAILED(hr))
	{
		SAFE_RELEASE(pVSCode);			//임시 개체 제거.
		return false;
	}

	//전역 참조 : : Input Layout 제작시 필요함.
	//m_pVSCode = pVSCode;

		
	
	//-------------------------
	// 픽셀 셰이더 로드.
	//-------------------------
	LPPIXELSHADER pPS = nullptr;
	ID3DBlob* pPSCode = nullptr;
	hr = ShaderCompile(fileName, "PS_Main", "ps_5_0", &pPSCode);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"[실패] ShaderLoad :: Pixel Shader 컴파일 실패", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// 픽셀 셰이더 객체 생성 Create the pixel shader
	hr = pDevice->CreatePixelShader(	pPSCode->GetBufferPointer(), 
										pPSCode->GetBufferSize(), 
										nullptr,
										&pPS
									);


	SAFE_RELEASE(pPSCode);				//임시 개체 제거.	


	//----------------
	// 이펙트 정보 구성.(완료)
	//----------------
	m_pVS = pVS;
	m_pPS = pPS;
	m_pVSCode = pVSCode;
	

	return true;
}


bool CShader::ShaderCompile(
						LPCWSTR fileName,		// 소스파일이름.
						char* entryPoint,		// 메인함수 진입점.
						char* shaderModel,		// 셰이더 모델.
						ID3DBlob** ppCode		// [출력] 컴파일된 셰이더 코드.
						)
{
	HRESULT hr = S_OK;
	ID3DBlob* pError = nullptr;

	//컴파일 옵션1.
	UINT Flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;		//열우선 행렬 처리. 구형 DX9 이전까지의 전통적인 방식. 속도가 요구된다면, "행우선" 으로 처리할 것.
														//UINT Flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;	//행우선 행렬 처리. 열 우선 처리보다 속도의 향상이 있지만, 행렬을 전치한수 GPU 에 공급해야 한다.
														//UINT Flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
	Flags |= D3DCOMPILE_DEBUG;							//디버깅 모드시 옵션 추가.
#endif

														//셰이더 소스 컴파일.
	hr = D3DCompileFromFile(fileName,
		nullptr, nullptr,
		entryPoint,
		shaderModel,
		Flags,				//컴파일 옵션.1
		0,					//컴파일 옵션2,  Effect 파일 컴파일시 적용됨. 이외에는 무시됨.
		ppCode,				//[출력] 컴파일된 셰이더 코드.
		&pError				//[출력] 컴파일 에러 코드.
	);
	if (FAILED(hr))
	{
		WindowErrorW(TRUE, L"Effect::셰이더 컴파일 실패", hr, pError, fileName, entryPoint, shaderModel);
		return false;
	}

	SAFE_RELEASE(pError);
	return true;
}


bool CShader::CreateCB()
{

}