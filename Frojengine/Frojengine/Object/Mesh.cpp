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
	// 2. ���̴� �� ��� ���� ����. --> ���� ���̴� �� ��� ���� ���
	//-----------------------
	pDXDC->VSSetShader(g_pVS, nullptr, 0);
	pDXDC->PSSetShader(g_pPS, nullptr, 0);
	pDXDC->VSSetConstantBuffers(0, 1, &g_pCB);


	//-----------------------
	// 3. ���� ���� ����
	//-----------------------
	UINT stride = sizeof(VertexFormat);
	UINT offset = 0;
	pDXDC->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	//�Է� ���̾ƿ� ����. Set the input layout
	pDXDC->IASetInputLayout(m_pInputLayout);
	// ���� ���� ���� ���� Set primitive topology
	pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//-----------------------
	// 4. �׸���!  
	//-----------------------
	pDXDC->Draw(m_VF.size(), 0);
}


bool CMesh::CreateBuffer(LPDEVICE device)
{

	//---------------------------
	// ���� ���� Vertex Buffer ����
	//---------------------------
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;									// ���� �����
	bd.ByteWidth = sizeof(VertexFormat) * m_VF.size();				// ���� ũ��
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;						// ���� �뵵 : "���� ����" ��� ���� 
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = &m_VF[0];											// ���ۿ� �� ������ ���� : "������"..


																	// ���� ���� ����.
	hr = device->CreateBuffer(&bd, &rd, &m_pVB);
	if (FAILED(hr))
		return false;


	//---------------------------
	// ���� �Է±��� Input layout 
	//---------------------------
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                       offset         classification             
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// ���� �Է±��� ��ü ���� Create the input layout
	hr = device->CreateInputLayout(layout,
		numElements,
		g_pVSCode->GetBufferPointer(),	// (����) ���̴� ���
		g_pVSCode->GetBufferSize(),		// (����) ���̴� ���
		&m_pInputLayout
	);
	if (FAILED(hr))
		return false;


	return true;
}