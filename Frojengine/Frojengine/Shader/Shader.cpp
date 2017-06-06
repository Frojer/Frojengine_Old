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

	// 정점 입력구조 생성
	//LoadLayout();

	// 상수 데이터 초기화
	ZeroMemory(&m_CBuffer, sizeof(CB_WVP));

	// 상수 버퍼 생성
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
	// 렌더링 옵션 : 외부에서 결정.
	//------------------------------------ 
	//... 필요시 추가 ...

	//------------------------------------
	// 메쉬 -디바이스에 등록 : 외부에서 결정.
	//------------------------------------
	// 정점 버퍼 설정 : 외부에서 결정 ... 
	//...
	// 입력 레이아웃 설정. 
	// m_pDXDC->IASetInputLayout(m_pLayout);
	// 기하 위상 구조 설정 : 외부에서 결정 ... 
	//...

	//------------------------------------
	// 셰이더 설정.
	//------------------------------------
	//상수 버퍼 설정 & 갱신.
	//...
	m_pDXDC->VSSetConstantBuffers(0, 1, &m_pCB);
	
	//셰이더 설정.
	m_pDXDC->VSSetShader(m_pVS, nullptr, 0);
	m_pDXDC->PSSetShader(m_pPS, nullptr, 0);
	

	//------------------------------------
	// 그리기! 
	//------------------------------------
	// ... 외부에서 결정 ... 
	 

	//------------------------------------
	// 렌더링 옵션 복구 : 타 이펙트/유닛 렌더링을 위한 장치 설정 리셋(옵션)
	//------------------------------------
	//... 외부에서 결정 / 필요시 추가 ...
}


void CShader::UpdateCB(MATRIXA* pWorld)
{
	// 외부 지정 행렬로 상수버퍼 갱신. 
	

	//-----------------------
	// 상수버퍼 갱신
	//-----------------------
	XMStoreFloat4x4(&m_CBuffer.mWorld, *pWorld);
	//m_CBuffer.mTM = XMMatrixTranspose(mTM);	//셰이더에서 '열 우선 Column major' 기준으로 처리하면 속도 향상을 기대할 수 있습니다. 이를 위한 행렬 전치 처리.	

	//셰이더 상수 버퍼 갱신.(동적버퍼)
	UpdateDynamicConstantBuffer(m_pCB, &m_CBuffer, sizeof(CB_WVP));
	//셰이더 상수 버퍼 갱신.(정적버퍼)
	//m_pDXDC->UpdateSubresource(m_pCB, 0, nullptr, &m_CBuffer, 0, 0);
}




////////////////////////////////////////////////////////////////////////////// 
//
// 셰이더 읽기 : 셰이더는 타 이펙트과 공유가 가능하도록 독립적 설계가 필요합니다.
//
// 각 이펙트당 전용 셰이더를 생성하거나 전체 엔진에서 공유되는 셰이더가 사용될 수 있습니다.
// 셰이더는 필요시 '교체가능'한 목록임을 상기합시다.
//
bool CShader::LoadShader(LPCWSTR fileName)
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
	hr = m_pDevice->CreateVertexShader(	pVSCode->GetBufferPointer(), 
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
	hr = m_pDevice->CreatePixelShader(	pPSCode->GetBufferPointer(), 
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



////////////////////////////////////////////////////////////////////////////// 
//
// 셰이더 소스 컴파일 : 셰이더 소스(*.fx)를 GPU 용 기계어로 변환합니다. 
//
bool CShader::ShaderCompile(
						LPCWSTR fileName,		// 소스파일이름.
						char* entryPoint,		// 메인함수 진입점.
						char* shaderModel,		// 셰이더 모델.
						LPXDATA* ppCode		// [출력] 컴파일된 셰이더 코드.
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



////////////////////////////////////////////////////////////////////////////// 
//
// (정적) 상수 버퍼 생성
//
bool CShader::CreateConstantBuffer(UINT size, LPBUFFER* ppCB)
{
	HRESULT hr;
	LPBUFFER pCB = nullptr;

	// 상수 버퍼 정보 설정
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	
	// 상수 버퍼 생성
	hr = m_pDevice->CreateBuffer(&bd, nullptr, &pCB);

	if (FAILED(hr))
	{
		WindowError(hr, L"Shader::CreateConstantBuffer : 상수버퍼 생성 실패");
		return false;
	}

	*ppCB = pCB;

	return true;
}




////////////////////////////////////////////////////////////////////////////// 
//
// (동적) 상수 버퍼 생성
//
bool CShader::CreateDynamicConstantBuffer(UINT size, LPVOID pData, LPBUFFER* ppCB)
{
	HRESULT hr = S_OK;
	LPBUFFER pCB = nullptr;

	// 상수 버퍼 정보 설정.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;					// 동적 정점버퍼 설정.
	bd.ByteWidth = size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		// CPU 접근 설정.


	// 서브리소스 설정.
	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = pData;				// (외부) 상수 데이터 설정.
	sd.SysMemPitch = 0;
	sd.SysMemSlicePitch = 0;

	// 상수 버퍼 생성.
	hr = m_pDevice->CreateBuffer(&bd, &sd, &pCB);
	if (FAILED(hr))
	{
		WindowError(hr, L"Shader::CreateDynamicConstantBuffer : 동적 상수버퍼 생성 실패");
		return false;
	}

	// 외부로 전달.
	*ppCB = pCB;

	return true;
}




////////////////////////////////////////////////////////////////////////////// 
//
// 동적 버퍼 갱신.
//
bool CShader::UpdateDynamicConstantBuffer(ID3D11Resource* pBuff, LPVOID pData, UINT size)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mr;
	ZeroMemory(&mr, sizeof(mr));

	// 상수버퍼 접근
	hr = m_pDXDC->Map(pBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
	if (FAILED(hr))
	{
		WindowError(hr, L"Effect::UpdateDynamicConstantBuffer : Map 실패");
		return false;
	}

	// 상수 버퍼 갱신.
	memcpy(mr.pData, pData, size);

	// 상수버퍼 닫기.
	m_pDXDC->Unmap(pBuff, 0);


	return true;
}