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
	// �� ��ü ����
	//---------------------

	// ���ϵ��� / �޽� �ε�.
	m_pMesh = pMesh;
	if (pMesh != nullptr)
	{
		pMesh->AddReference();
	}

	// ���� ����
	result = CreateBuffer();

	if (!result)
	{
		return false;
	}

	// ���׸��� �ε�
	result = LoadMaterial(pMaterial);

	if (!result)
	{
		return false;
	}

	// ���� �Է±��� ����.
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
	// ���� ���� Vertex Buffer ����
	//---------------------------
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;									// ���� �����
	bd.ByteWidth = sizeof(VertexFormat) * m_pMesh->GetVertexSize();				// ���� ũ��
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;						// ���� �뵵 : "���� ����" ��� ���� 
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = &m_pMesh->m_Vertices[0];											// ���ۿ� �� ������ ���� : "������"..


																	// ���� ���� ����.
	hr = m_pDevice->CreateBuffer(&bd, &rd, &m_pVB);

	if (FAILED(hr))
	{
		return false;
	}


	// �ε��� ���� Index Buffer ����
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * m_pMesh->GetIndexSize();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &m_pMesh->m_Indices[0];

	// �ε��� ���� ����
	hr = m_pDevice->CreateBuffer(&bd, &InitData, &m_pIB);
	if (FAILED(hr))
		return false;


	m_Stride = sizeof(VertexFormat);
	m_Offset = 0;

	return true;
}



////////////////////////////////////////////////////////////////////////////// 
//
// ���̴� �б� : ���̴��� Ÿ �𵨰� ������ �����ϵ��� ������ ���谡 �ʿ��մϴ�.
//
bool CModel::LoadMaterial(CMaterial* pMaterial)
{

	// �� �𵨴� ���� ���̴��� �����ϰų� ��ü �������� �����Ǵ� ���̴��� ���� �� �ֽ��ϴ�.
	// ���̴��� �ʿ�� '��ü����'�� ������� ����սô�.
	//
	// �̹� ���𿡼��� ������ �ε��� �����͸� "����", �����ϰڽ��ϴ�.
	// ... 

	// ���̴� �ε�. --> Effect Ŭ������ ����.��

	//----------------
	// �� ���� ���� : �̹� ���𿡼��� ������ �ε��� �����͸� "����", �����ϰڽ��ϴ�.
	//----------------

	// ���̴��� �ܺο��� ����, Ÿ �� �������� ���� (����) ��
	m_pMaterial = pMaterial;


	return true;
}



////////////////////////////////////////////////////////////////////////////// 
//
// �Է� �����̿� ����  : ������ �Է� ���̾ƿ��� ���� �԰��� ���̴���� ������ �����մϴ�.
//
bool CModel::LoadLayout()
{
	HRESULT hr = S_OK;


	// ���� �Է� ���̾ƿ��� �������� �� ���̴� ������ ������ �Է±����� �����ϴ� ��ġ�Դϴ�. 
	// ������, ���̴����� ������ �ʿ��ϰ� ���̴� ������ ������ ��ġ�ϰ� �����ϴ� ���� �Ϲ����̹Ƿ� 
	// ���̴� Ŭ�������� �Է·��̾ƿ� ��ü�� �����Ǵ� �������� ��õ�մϴ�.
	//  
	// �̹� ���𿡼��� �Ϲ� �Լ����·� �ܼ��ϰ� �����ϵ��� �ϰڽ��ϴ�.
	// ... 

	// ���� �Է±��� Input layout
	// GPU �� ���޵� ���ϵ����� - ���� ������ ������ ������ �뵵���� ������ �����մϴ�.
	// ���� Vertex Format(DX7/8/9) �Ǵ� Vertex Declaration(DX9) �� ���� �������� �������� 
	// ���� ������ ����� �䱸�� ���߾� ������ �� ��������� Ȯ��Ǿ����ϴ�.
	//
	// �ٸ� ������ ����� ���ؼ��� �Ʒ��� ������ ���� �Ǵ� ȣȯ�Ǿ�� �մϴ�.
	// 1.���� ������ ������.  Vertex Buffer Data
	// 2.���� ���� Vertex Format (Input Layout)
	// 3.���̴� �Լ��� �Է±���.  Vertex Shader (Input Layout)
	//
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//  Sementic          format                       offset         classification             
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 	//DWORD �� ����		
		//{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	// ���� �Է±��� ��ü ���� Create the input layout
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
	if (IsKeyDown(VK_F9))		//�� ���� ����.
	{
		//... 
	}


	//-----------------------
	// �ִϸ��̼� ó��.
	//-----------------------
	//...



	//-----------------------
	// ���׸��� ����
	//-----------------------
	m_pMaterial->Update();
}




/////////////////////////////////////////////////////////////////////////////
//
void CModel::UpdateCB(MATRIXA* mTM, CLight* light)
{

	//�ܺ� ���� ��ķ� ������� ����. 


	//-----------------------
	// ������� ����
	//-----------------------
	m_pMaterial->UpdateCB(mTM, light);
}




void CModel::Draw()
{
	//Z-Buffer(Depth) ���� Ȱ��ȭ : Z-Test ON, Z-Write ON (�⺻��)
	//��ü �׸����, �ʿ��ϴٸ�, On/Off �����մϴ�.
	//.. 


	//-----------------------
	// ���� ���� ����
	//-----------------------

	//----------------------------------------------------------------------
	// �ٸ� ������ ����� ���ؼ��� �Ʒ��� ������ ���� �Ǵ� ȣȯ�Ǿ�� �մϴ�.
	// 1.���� ������ ������.  Vertex Buffer Data
	// 2.���� ���� Vertex Format (Input Layout)
	// 3.���̴� �Լ��� �Է±���.  Vertex Shader (Input Layout)
	// 4.���� ��ȯ ó�� Vertex Transform
	//----------------------------------------------------------------------
	// ���� ���� ����. Set vertex buffer
	// ����̽��� �Է�-���� �������� (Input-Assembler Stage) �� �������۸� ����.(Binding)
	//-----------------------
	// ���� ���� ����
	// �޽�- ĳ���� - �� ����̽��� ����Ѵ�. 
	//-----------------------
	m_pDXDC->IASetVertexBuffers(0, 1, &m_pVB, &m_Stride, &m_Offset);
	m_pDXDC->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

	// �Է� ���̾ƿ� ����. Set the input layout
	m_pDXDC->IASetInputLayout(m_pInputLayout);
	// ���� ���� ���� ���� Set primitive topology
	m_pDXDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//------------------------------------
	// ���̴� ����.
	//------------------------------------
	// ��� ���� ���� & ����.
	// ���̴� ���� ...
	
	m_pMaterial->Apply();

	//-----------------------
	// �׸���!  
	//-----------------------
	m_pDXDC->DrawIndexed(m_pMesh->GetIndexSize(), 0, 0);


	// ������ �ɼ� ���� : Ÿ ��/���� �������� ���� ��ġ ���� ����(�ɼ�)
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