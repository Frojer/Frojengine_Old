#include "Mesh.h"
#include "..\..\Shader.h"

CMesh::CMesh()
{
	m_pVB = nullptr;
	m_pIB = nullptr;
	m_pInputLayout = nullptr;
}

CMesh::CMesh(const CMesh& obj)
{

}

CMesh::~CMesh()
{

}


bool CMesh::Create(LPDEVICE device, LPCWSTR fileName)
{
	bool result;



	result = CreateBuffer(device);

	if (!result)
	{
		return false;
	}

	return true;
}


void CMesh::Release()
{
	if (m_pInputLayout != nullptr)
	{
		m_pInputLayout->Release();
		m_pInputLayout = nullptr;
	}

	if (m_pVB != nullptr)
	{
		m_pVB->Release();
		m_pVB = nullptr;
	}
}


void CMesh::Draw(LPDXDC pDXDC)
{
	//-----------------------
	// 2. 셰이더 및 상수 버퍼 설정. --> 공유 셰이더 및 상수 버퍼 사용
	//-----------------------
	pDXDC->VSSetShader(g_pVS, nullptr, 0);
	pDXDC->PSSetShader(g_pPS, nullptr, 0);
	pDXDC->VSSetConstantBuffers(0, 1, &g_pCB);


	//-----------------------
	// 3. 기하 정보 설정
	//-----------------------
	UINT stride = sizeof(VertexFormat);
	UINT offset = 0;
	pDXDC->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	//입력 레이아웃 설정. Set the input layout
	pDXDC->IASetInputLayout(m_pInputLayout);
	// 기하 위상 구조 설정 Set primitive topology
	pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//-----------------------
	// 4. 그리기!  
	//-----------------------
	pDXDC->Draw(m_VF.size(), 0);
}


bool CMesh::CreateBuffer(LPDEVICE device)
{

	//---------------------------
	// 정점 버퍼 Vertex Buffer 생성
	//---------------------------
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;									// 버퍼 사용방식
	bd.ByteWidth = sizeof(VertexFormat) * m_VF.size();				// 버퍼 크기
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;						// 버퍼 용도 : "정점 버퍼" 용로 설정 
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = &m_VF[0];											// 버퍼에 들어갈 데이터 설정 : "정점들"..


																	// 정점 버퍼 생성.
	hr = device->CreateBuffer(&bd, &rd, &m_pVB);
	if (FAILED(hr))
		return false;


	//---------------------------
	// 정점 입력구조 Input layout 
	//---------------------------
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                       offset         classification             
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// 정접 입력구조 객체 생성 Create the input layout
	hr = device->CreateInputLayout(layout,
		numElements,
		g_pVSCode->GetBufferPointer(),	// (공통) 셰이더 사용
		g_pVSCode->GetBufferSize(),		// (공통) 셰이더 사용
		&m_pInputLayout
	);
	if (FAILED(hr))
		return false;


	return true;
}