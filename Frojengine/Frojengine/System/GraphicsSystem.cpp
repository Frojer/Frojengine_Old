#include "GraphicsSystem.h"

CGraphicsSystem::CGraphicsSystem()
{
	m_pDevice = nullptr;
	m_pDXDC = nullptr;
	m_pSwapChain = nullptr;
	m_pRTView = nullptr;

	ZeroMemory(&m_Mode, sizeof(m_Mode));
}

CGraphicsSystem::CGraphicsSystem(const CGraphicsSystem& obj)
{

}

CGraphicsSystem::~CGraphicsSystem()
{

}



bool CGraphicsSystem::Create(HWND hWnd, int width, int height)
{
	bool result;

	m_Mode.Width = width;
	m_Mode.Height = height;
	m_Mode.RefreshRate.Numerator = 0;
	m_Mode.RefreshRate.Denominator = 1;
	m_Mode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	result = CreateDX(hWnd);

	if (!result)
	{
		return false;
	}

	return true;
}



///////////////////////////////////////////////////////////////////////////// 
//
// ����̽� ��ü �� ���� ��ġ ���� : ����� 1ȸ ȣ��.  
// �⺻ ������ ��ü�� ���Ž� ������ �������� ó���մϴ�.(����)
//
void CGraphicsSystem::Release()
{
	// ��ġ ���� ���� : ���� ���� �ʱ�ȭ�� �ؾ� �մϴ�. (�޸� ���� ����)
	if (m_pDXDC)
		m_pDXDC->ClearState();
	
	//if (m_pSwapChain) m_pSwapChain->SetFullscreenState(false, NULL);

	SAFE_RELEASE(m_pRTView);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pDXDC);
	SAFE_RELEASE(m_pDevice);
}




///////////////////////////////////////////////////////////////////////////// 
//
// DX �� ���� ��ü �ʱ�ȭ : ���ø����̼� ���۽� 1ȸ ȣ��.
//
// 1.����̽� ���� 
// 2.����ü�� �� ����Ÿ�� ����.
// 3.����/���ٽ� ���� ����.
// 4.��Ÿ ������ ���� ��ġ ����.
//
bool CGraphicsSystem::CreateDX(HWND hWnd)
{
	bool result;

	//----------------------------------------
	// 1�ܰ� : ������ ��ġ ���� �ʼ� ����
	//----------------------------------------
	
	// D3D ������ ��ġ Device �� ����ü�� Swap Chain ����. 
	result = CreateDeviceSwapChain(hWnd);

	if (!result)
	{
		return false;
	}

	// ��ġ-����ü���� ����Ÿ��(�����) ȹ��
	result = CreateRenderTarget();
	
	if (!result)
	{
		return false;
	}

	// ��ġ ��º��ձ�(Output Merger) �� ���͸� Ÿ�� �� ����-���ٽ� ���� ���.
	m_pDXDC->OMSetRenderTargets(
		1,				// ����Ÿ�� ����.(max: D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
		&m_pRTView,		// ����Ÿ��("�����") ���.	
		nullptr
	);

	// ����Ʈ ����
	SetViewPort();


	//----------------------------------------
	// 2�ܰ� : ��Ÿ ������ ���� �߰� ����.
	//----------------------------------------
	// �������� �ʿ��� ����� ��ü���� ����/���� �մϴ�.
	// ī�޶�, ����, ��Ʈ, ���̴� ���...



	// �۾� �Ϸ�, ��ġ �غ� �Ϸ�.
	return true;
}




///////////////////////////////////////////////////////////////////////////// 
//
// ����̽� ��ü �� ���� ��ġ ���� : ����� 1ȸ ȣ��.  
// �⺻ ������ ��ü�� ���Ž� ������ �������� ó���մϴ�.(����)
//
void CGraphicsSystem::ReleaseDX()
{

}



/////////////////////////////////////////////////////////////////////////////
//
// D3D ������ '��ġ(Device)' �� ����ü�� ����. 
//
bool CGraphicsSystem::CreateDeviceSwapChain(HWND hWnd)
{
	HRESULT hr = S_OK;
	
	// D3D ��� ���� (Direct3D feature level) : DX11 ������ HW ������ ȣȯ�� ���
	D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_0;		//DX11 ����.
	//D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_9_3;		//DX9.0c ����.


	// ��ġ (Device) �� ����ü��(SwapChain) ���� ����.
	// ����ü���� ���߹��۸� �ý����� ���ϸ�
	// �������� '�ø���Flipping' ü�ΰ� ������ �ǹ��Դϴ�.  
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Windowed = TRUE;							// Ǯ��ũ�� �Ǵ� â��� ����.
	sd.OutputWindow = hWnd;						// ����� ������ �ڵ�.
	sd.BufferCount = 1;							// ����� ����.
	//sd.BufferDesc = m_Mode;
	sd.BufferDesc.Width = m_Mode.Width;			// �ػ� ����.(����� ũ��)
	sd.BufferDesc.Height = m_Mode.Height;
	sd.BufferDesc.Format = m_Mode.Format;		// ����� ����԰� (A8R8G8B8) â��忡���� ���� ���� 
	sd.BufferDesc.RefreshRate.Numerator = 0;    // ���� ������.(��������ȭ VSync Ȱ��ȭ�� ǥ�ذ����� ���� : 60hz)
	//sd.BufferDesc.RefreshRate.Numerator = 0;	// ���� ������.(��������ȭ VSync Off)
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// �뵵 ����: '����Ÿ��' 
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//sd.Flags = 0;
	sd.SampleDesc.Count = 1;					// AA ����
	sd.SampleDesc.Quality = 0;
	
	
	
	// D3D ������ '��ġ(Device)' �� ����ü�� ����. 
	//
	hr = D3D11CreateDeviceAndSwapChain(
			NULL,							// ���� ��� ������ (�⺻��ġ�� NULL)
			D3D_DRIVER_TYPE_HARDWARE,		// HW ���� 
			NULL,							// SW Rasterizer DLL �ڵ�.  HW ���ӽÿ��� NULL.
			0,								// ����̽� ���� �÷���.(�⺻��)
			//D3D11_CREATE_DEVICE_DEBUG,	// ����̽� ���� �÷���.(�����)
			&featureLevels,					// (������) ����̽� ��� ����(Feature Level) �迭
			1,								// (������) ����̽� ��� ����(Feature Level) �迭 ũ��.
			D3D11_SDK_VERSION,				// DX SDK ����.
			&sd,							// ����̽� ���� �� �ɼ�.
			&m_pSwapChain,					// [���] ����ü�� �������̽� ���.
			&m_pDevice,						// [���] ����̽� �������̽� ���.
			NULL,							// [���] (������) ����̽� ��� ����. �ʿ���ٸ� NULL ����. 
			&m_pDXDC						// [���] ����̽� ���ؽ�Ʈ ���.
	);

	if (FAILED(hr))
	{
		WindowError(hr, L"����̽� / ����ü�� ���� ����.");
		return false;
	}
	
	return true;
}



/////////////////////////////////////////////////////////////////////////////
//
//  ��ġ-����ü���� ����Ÿ��(�����) ȹ��
//
//  ���ҽ���(Resource View)  
//  DX11 �� ���ҽ��� "����" �������� �����ϰ� ����Ǿ� �ֽ��ϴ�. 
//  (�޸� ������ ȿ���� ���� �� �ٿ뵵 Ȱ���� ����..)
//  ���� ����� �� �뵵�� �´� ���ٹ��(�������̽�)�� ȹ��(����)�ؾ� �մϴ�.
//  �� �۾��� "���ҽ� �� Resource View" �� ���� ó���մϴ�.
//                     
//  ����ü�ο� ����(Bind)�� ������ ��¹��� - "����� BackBuffer" �� 2D ��� ���ҽ�(�ؽ�ó) �̸�
//  "����Ÿ�� RenderTarget" ������ ���ҽ��並 ����, �ش� ���۸� ����/��� �ϰڽ��ϴ�.
//
bool CGraphicsSystem::CreateRenderTarget()
{
	HRESULT hr = S_OK;

	// ������ ȹ��.
	ID3D11Texture2D *pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);		// ����� ���, 2D �ؽ��� ����, ȭ����¿�

	if (FAILED(hr))
		return false;

	//ȹ���� ����ۿ� ����Ÿ�� �� ����.(����Ÿ��'��'���� ������)
	ID3D11RenderTargetView* pRTView;
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRTView);		// '����Ÿ�ٺ�'�� ����.

	if (FAILED(hr))
	{
		WindowError(hr, L"�����-����Ÿ�� �� ���� ����.");
		return false;
	}

	//���ҽ� �� ���� ��, ���ʿ��� DX �ڵ��� �����ؾ� �մϴ�.(�޸� ���� ����)
	SAFE_RELEASE(pBackBuffer);

	return true;
}




/////////////////////////////////////////////////////////////////////////////
//
// ����Ʈ ���� :  DX11 ������ �⺻ó�� ���� ������ ����ڰ� ���� �����ؾ��մϴ� 
//			     (OpenGL ������ �������� �̷��� �� �Խ��ϴ�.)
//
void CGraphicsSystem::SetViewPort()
{
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (FLOAT)m_Mode.Width;
	vp.Height = (FLOAT)m_Mode.Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_pDXDC->RSSetViewports(1, &vp);
}




/////////////////////////////////////////////////////////////////////////////
//
//  ��ġ-����ü��/����Ÿ��/����/���ٽ� ���� Ŭ���� 
//
void CGraphicsSystem::ClearBackBuffer()
{
	m_pDXDC->ClearRenderTargetView(m_pRTView, (float*)&m_BackColor);
}



/////////////////////////////////////////////////////////////////////////////
//
//  ��ġ-����ü�� ��ȯ : ��� �׸��� �Ϸ�� �� ȣ���ؾ� ��.
//
void CGraphicsSystem::Flip()
{
	m_pSwapChain->Present(0, 0);			// ȭ�� ���
}



////////////////////////////////////////////////
//
//   Get, Set �Լ��� ����
//
////////////////////////////////////////////////
#pragma region GetSet

void CGraphicsSystem::SetBackColor(COLOR backColor)
{
	m_BackColor = backColor;
}



COLOR CGraphicsSystem::GetBackColor()
{
	return m_BackColor;
}



void CGraphicsSystem::SetWindowMode(bool bWindow)
{
	m_bWindowMode = bWindow;
}



bool CGraphicsSystem::GetWindowMode()
{
	return m_bWindowMode;
}



void CGraphicsSystem::SetVSync(bool bVSync)
{
	m_bVSync = bVSync;
}



bool CGraphicsSystem::GetVSync()
{
	return m_bVSync;
}

#pragma endregion