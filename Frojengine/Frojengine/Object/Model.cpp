#include "..\Frojengine.h"

CModel::CModel()
{
	m_pDevice = nullptr;
	m_pDXDC = nullptr;

	m_pVB = nullptr;
	m_pIB = nullptr;
	m_pInputLayout = nullptr;

	m_pMaterial = nullptr;
}

CModel::CModel(const CModel& obj)
{

}

CModel::~CModel()
{

}


bool CModel::Create(LPDEVICE pDevice, CMesh* pMesh, CMaterial* pMaterial)
{
	bool result;

	m_pDevice = pDevice;
	m_pDevice->GetImmediateContext(&m_pDXDC);

	//---------------------
	// 모델 객체 생성
	//---------------------

	// 기하도형 / 메쉬 로드.
	m_pMesh = pMesh;
	if (pMesh != nullptr)
	{
		pMesh->AddReference();
	}

	// 버퍼 생성
	result = CreateBuffer();

	if (!result)
	{
		return false;
	}

	// 머테리얼 로드
	result = LoadMaterial(pMaterial);

	if (!result)
	{
		return false;
	}

	// 정점 입력구조 생성.
	LoadLayout();

	return true;
}


void CModel::Release()
{
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pInputLayout);
}


bool CModel::CreateBuffer()
{
	//---------------------------
	// 정점 버퍼 Vertex Buffer 생성
	//---------------------------
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;									// 버퍼 사용방식
	bd.ByteWidth = sizeof(VertexFormat) * m_pMesh->GetVertexSize();				// 버퍼 크기
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;						// 버퍼 용도 : "정점 버퍼" 용로 설정 
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = &m_pMesh->m_Vertices[0];											// 버퍼에 들어갈 데이터 설정 : "정점들"..


																	// 정점 버퍼 생성.
	hr = m_pDevice->CreateBuffer(&bd, &rd, &m_pVB);

	if (FAILED(hr))
	{
		return false;
	}


	// 인덱스 버퍼 Index Buffer 생성
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * m_pMesh->GetIndexSize();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &m_pMesh->m_Indices[0];

	// 인덱스 버퍼 생성
	hr = m_pDevice->CreateBuffer(&bd, &InitData, &m_pIB);
	if (FAILED(hr))
		return false;


	m_Stride = sizeof(VertexFormat);
	m_Offset = 0;

	return true;
}



////////////////////////////////////////////////////////////////////////////// 
//
// 셰이더 읽기 : 셰이더는 타 모델과 공유가 가능하도록 독립적 설계가 필요합니다.
//
bool CModel::LoadMaterial(CMaterial* pMaterial)
{

	// 각 모델당 전용 셰이더를 생성하거나 전체 엔진에서 공유되는 셰이더가 사용될 수 있습니다.
	// 셰이더는 필요시 '교체가능'한 목록임을 상기합시다.
	//
	// 이번 데모에서는 기존에 로딩된 데이터를 "공유", 재사용하겠습니다.
	// ... 

	// 셰이더 로드. --> Effect 클래스로 통합.★

	//----------------
	// 모델 정보 설정 : 이번 데모에서는 기존에 로딩된 데이터를 "공유", 재사용하겠습니다.
	//----------------

	// 셰이더는 외부에서 공급, 타 모델 정보에서 공유 (가능) ★
	m_pMaterial = pMaterial;


	return true;
}



////////////////////////////////////////////////////////////////////////////// 
//
// 입력 레아이웃 생성  : 생성된 입력 레이아웃은 동일 규격의 셰이더들과 공유가 가능합니다.
//
bool CModel::LoadLayout()
{
	HRESULT hr = S_OK;


	// 정점 입력 레이아웃은 정점버퍼 와 셰이더 사이의 데이터 입력구조을 결정하는 장치입니다. 
	// 생성시, 셰이더측의 정보가 필요하고 셰이더 내부의 구조와 일치하게 구성하는 것이 일반적이므로 
	// 셰이더 클래스에서 입력레이아웃 객체가 관리되는 설계방식을 추천합니다.
	//  
	// 이번 데모에서는 일반 함수형태로 단순하게 관리하도록 하겠습니다.
	// ... 

	// 정점 입력구조 Input layout
	// GPU 에 공급될 기하데이터 - 개별 정점의 데이터 구조와 용도등의 정보를 구성합니다.
	// 구형 Vertex Format(DX7/8/9) 또는 Vertex Declaration(DX9) 과 동일 목적으로 사용되지만 
	// 신형 렌더링 기술의 요구에 맞추어 구조적 및 기능적으로 확장되었습니다.
	//
	// 바른 렌더링 결과를 위해서는 아래의 조건이 동일 또는 호환되어야 합니다.
	// 1.정점 버퍼의 데이터.  Vertex Buffer Data
	// 2.정점 구조 Vertex Format (Input Layout)
	// 3.셰이더 함수의 입력구조.  Vertex Shader (Input Layout)
	//
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                       offset         classification             
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 	//DWORD 형 색상		
		//{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	// 정접 입력구조 객체 생성 Create the input layout
	hr = m_pDevice->CreateInputLayout(layout,
		numElements,
		m_pMaterial->GetShader()->GetBufferPointer(),
		m_pMaterial->GetShader()->GetBufferSize(),
		&m_pInputLayout
	);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}



void CModel::Update(float deltaTime)
{
	if (IsKeyDown(VK_F9))		//모델 정보 리셋.
	{
		//... 
	}


	//-----------------------
	// 애니메이션 처리.
	//-----------------------
	//...



	//-----------------------
	// 머테리얼 갱신
	//-----------------------
	m_pMaterial->Update();
}




/////////////////////////////////////////////////////////////////////////////
//
void CModel::UpdateCB(MATRIXA* mTM, CLight* light)
{

	//외부 지정 행렬로 상수버퍼 갱신. 


	//-----------------------
	// 상수버퍼 갱신
	//-----------------------
	m_pMaterial->UpdateCB(mTM, light);
}




void CModel::Draw()
{
	//Z-Buffer(Depth) 연산 활성화 : Z-Test ON, Z-Write ON (기본값)
	//개체 그리기시, 필요하다면, On/Off 가능합니다.
	//.. 


	//-----------------------
	// 기하 정보 설정
	//-----------------------

	//----------------------------------------------------------------------
	// 바른 렌더링 결과를 위해서는 아래의 조건이 동일 또는 호환되어야 합니다.
	// 1.정점 버퍼의 데이터.  Vertex Buffer Data
	// 2.정점 구조 Vertex Format (Input Layout)
	// 3.셰이더 함수의 입력구조.  Vertex Shader (Input Layout)
	// 4.각종 변환 처리 Vertex Transform
	//----------------------------------------------------------------------
	// 정점 버퍼 설정. Set vertex buffer
	// 디바이스의 입력-조립 스테이지 (Input-Assembler Stage) 에 정점버퍼를 연결.(Binding)
	//-----------------------
	// 기하 정보 설정
	// 메쉬- 캐릭터 - 를 디바이스에 등록한다. 
	//-----------------------
	m_pDXDC->IASetVertexBuffers(0, 1, &m_pVB, &m_Stride, &m_Offset);
	m_pDXDC->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

	// 입력 레이아웃 설정. Set the input layout
	m_pDXDC->IASetInputLayout(m_pInputLayout);
	// 기하 위상 구조 설정 Set primitive topology
	m_pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//------------------------------------
	// 셰이더 설정.
	//------------------------------------
	// 상수 버퍼 설정 & 갱신.
	// 셰이더 설정 ...
	
	m_pMaterial->Apply();

	//-----------------------
	// 그리기!  
	//-----------------------
	m_pDXDC->DrawIndexed(m_pMesh->GetIndexSize(), 0, 0);


	// 렌더링 옵션 복구 : 타 모델/유닛 렌더링을 위한 장치 설정 리셋(옵션)
	// ... 
}



void CModel::ChangeMesh(CMesh* pMesh)
{
	SAFE_RELEASE(m_pMesh);

	m_pMesh = pMesh;
	m_pMesh->AddReference();
}



void CModel::ChangeMaterial(CMaterial* pMaterial)
{
	if (m_pMaterial != nullptr)
	{
		m_pMaterial->Release();
		delete m_pMaterial;
	}

	m_pMaterial = pMaterial;
}