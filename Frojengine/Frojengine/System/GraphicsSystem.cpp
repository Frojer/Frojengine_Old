#include "GraphicsSystem.h"

COLOR CGraphicsSystem::m_BackColor = COLOR(0.0f, 0.0f, 0.0f, 1.0f);
bool CGraphicsSystem::m_bWindowMode = true;
bool CGraphicsSystem::m_bVSync = false;
bool CGraphicsSystem::m_bCullBack = false;
bool CGraphicsSystem::m_bWireFrame = false;
bool CGraphicsSystem::m_bZEnable = true;
bool CGraphicsSystem::m_bZWrite = true;
MATRIX CGraphicsSystem::mView;
MATRIX CGraphicsSystem::mProj;

CGraphicsSystem::CGraphicsSystem()
{
	m_pDevice = nullptr;
	m_pDXDC = nullptr;
	m_pSwapChain = nullptr;
	m_pRTView = nullptr;

	m_pDS = nullptr;
	m_pDSView = nullptr;

	m_RMode = RM_DEFAULT;

	m_BackColor = VECTOR4(0, 0.125f, 0.3f, 1);

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
	ReleaseDX();
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

	// ����/���ٽ� ���� ����.
	result = CreateDepthStencil();

	if (!result)
	{
		return false;
	}

	// ��ġ ��º��ձ�(Output Merger) �� ���͸� Ÿ�� �� ����-���ٽ� ���� ���.
	m_pDXDC->OMSetRenderTargets(
		1,				// ����Ÿ�� ����.(max: D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
		&m_pRTView,		// ����Ÿ��("�����") ���.	
		m_pDSView
	);

	// ����Ʈ ����
	//SetViewPort();


	//----------------------------------------
	// 2�ܰ� : ��Ÿ ������ ���� �߰� ����.
	//----------------------------------------
	// �������� �ʿ��� ����� ��ü���� ����/���� �մϴ�.
	// ī�޶�, ����, ��Ʈ, ���̴� ���...
	StateObjectCreate();


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
	// ��ġ ���� ���� : ���� ���� �ʱ�ȭ�� �ؾ� �մϴ�. (�޸� ���� ����)
	if (m_pDXDC)
		m_pDXDC->ClearState();

	// ���� ��ü ����.
	StateObjectRelease();

	//if (m_pSwapChain) m_pSwapChain->SetFullscreenState(false, NULL);

	SAFE_RELEASE(m_pDS);
	SAFE_RELEASE(m_pDSView);
	SAFE_RELEASE(m_pRTView);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pDXDC);
	SAFE_RELEASE(m_pDevice);
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
	sd.Windowed = m_bWindowMode;							// Ǯ��ũ�� �Ǵ� â��� ����.
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
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRTView);		// '����Ÿ�ٺ�'�� ����.

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
// ����-���ٽǹ��� ����. : DX11 ������ ����������-����Ÿ�ٺ�- �� ����������, 
//                       ����-���ٽǹ��ۿ��� ���� ������ �մϴ�.
//                       ����̽��� ��ϵ� ���� �����Դϴ�.
//
bool CGraphicsSystem::CreateDepthStencil()
{
	HRESULT hr;

	// ����/���ٽ� ���ۿ� ���� ����.
	D3D11_TEXTURE2D_DESC   td;
	ZeroMemory(&td, sizeof(td));
	td.Width = m_Mode.Width;
	td.Height = m_Mode.Height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D32_FLOAT;			// 32BIT. ���� ����.
	td.SampleDesc.Count = 1;					// AA ���� - RT �� ���� �԰� �ؼ�.
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// ����-���ٽ� ���ۿ����� ����.
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	// ���� ���� ����.
	hr = m_pDevice->CreateTexture2D(&td, NULL, &m_pDS);
	if (FAILED(hr)) return false;



	// ����-���ٽǹ��ۿ� ���ҽ� �� ���� ����. 
	D3D11_DEPTH_STENCIL_VIEW_DESC  dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.Format = td.Format;
	dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // AA ����.
	dd.Texture2D.MipSlice = 0;

	// ����-���ٽ� ���� �� ����.
	hr = m_pDevice->CreateDepthStencilView(m_pDS, &dd, &m_pDSView);
	if (FAILED(hr))
	{
		WindowError(hr, L"����/���ٽ� ���� �� ���� ����.");
		return false;
	}

	// ���ҽ� �� ���� ��, ���ʿ��� DX �ڵ��� �����ؾ� �մϴ�.(�޸� ���� ����)
	// SAFE_RELEASE(pDS);

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



////////////////////////////////////////////////////////////////////////////// 
//
// ��ġ ������ ���� ��ü ����.
//
//----------------------------------------------------------------------------
// ���� ��ü State Objects (DX10/11)
// DX10 ���� ���� TnL �� RenderState �� ���ŵǾ����ϴ�.
// �̸� ����ϴ� ���� ���°�ü State Objects �������̽���, ������ ���¸� �ϳ��� �׷����� 
// ���� �������� ����̽��� �����մϴ�.  �̸� ���� ��ġ�� ��� ���� ��ȭ�� �ѹ��� �����Ͽ� 
// ���ʿ��� �������(Overhead) �� ���̰� ���� ȿ������ �������� ������ �մϴ�.
//
// ���°�ü�� ���� �ʱ�� ������ ����ϱ⸦ �����ϸ� ����� ����(Release) �ؾ� �մϴ�.
// ���°�ü�� �����Ұ���(Immutable, �б�����) ��ü �Դϴ�.
// DX9 ������ State-Block �� �̿� ������ ����� ����߾����ϴ�.
//
// ���� ��ü �������̽� �� '�����Ͷ����� �������� Rasterize Stage' ���� ������ 
// ID3D11RasterizerState �������̽��� ���� ó���մϴ�.  
// �����ϰ� ������ ����/��� ��� ������ �����սô�.  �ڼ��� ���� ������ �����Ͻʽÿ�. 
// ��ũ1 : ���� ��ü https://msdn.microsoft.com/en-us/library/windows/desktop/bb205071(v=vs.85).aspx
// ��ũ2 : ���̹��� ���� �����ϱ� https://msdn.microsoft.com/ko-kr/library/windows/desktop/bb205074(v=vs.85).aspx#Create_Depth_Stencil_State 
//----------------------------------------------------------------------------
//
void CGraphicsSystem::StateObjectCreate()
{

	//----------------------------
	// ������ ���� ��ü ���� : "�����Ͷ����� �������� Rasterize Stage" ���� ����.
	//----------------------------
	RasterStateLoad();

	//----------------------------
	// ����/���ٽ� ���� ��ü ����.: "��º��ձ� Output Merger" ���� ����. 
	//----------------------------
	DSStateLoad();



	//----------------------------
	// ���ĺ��� ���� ��ü ���� : "��º��ձ� Output Merger" ���� ����. 
	//----------------------------
	//...

}



void CGraphicsSystem::StateObjectRelease()
{
	RasterStateRelease();
	DSStateRelease();
}




////////////////////////////////////////////////////////////////////////////// 
// 
// ������ ���� ��ü ����.
//
void CGraphicsSystem::RasterStateLoad()
{
	//�⺻ ������ ���� ��ü.
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;		// �ﰢ�� ���� ä���.(�⺻��)
	rd.CullMode = D3D11_CULL_NONE;		// �ø� ����. (�⺻���� �ø� Back)		
	rd.FrontCounterClockwise = false;   // ���� �⺻��...
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = false;
	rd.AntialiasedLineEnable = false;
	//�����Ͷ����� ��ü ����.
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_DEFAULT]);

	// ���̾� ������ �׸���. 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	//�����Ͷ����� ��ü ����.
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_WIREFRM]);

	// �ø� On! "CCW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_CULLBACK]);

	//���̾� ������ + �ø� On! "CCW"
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_WIRECULLBACK]);
}




////////////////////////////////////////////////////////////////////////////// 
//
// ������ ���� ��ü ���� : ���ø����̼� ����� ȣ��.
//
void CGraphicsSystem::RasterStateRelease()
{
	for (int i = 0; i<RS_MAX_; i++)
	{
		SAFE_RELEASE(m_RState[i]);
	}

}





void CGraphicsSystem::DSStateLoad()
{
	//----------------------------
	// ������ ���� ��ü ���� : "�����Ͷ����� �������� Rasterize Stage" ���� ����.
	//----------------------------
	//...


	//----------------------------
	// ����/���ٽ� ���� ��ü ����.: "��º��ձ� Output Merger" ���� ����. 
	//----------------------------
	//...	 
	D3D11_DEPTH_STENCIL_DESC  ds;
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS;
	ds.StencilEnable = FALSE;
	//ds.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	//ds.StnecilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	//...���� �⺻��, ����... 
	//...
	//ù��° ���� ��ü : Z-Test ON! (�⺻��)
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[SO_DEPTH_ON]);

	//�ι�° ���� ��ü : Z-Test OFF ����.
	ds.DepthEnable = FALSE;
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[SO_DEPTH_OFF]);

	//����° ���� ��ü : Z-Test On + Z-Write OFF.
	// Z-Test (ZEnable, DepthEnable) �� ������, Z-Write ���� ��Ȱ��ȭ �˴ϴ�.
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;		//���̰� ���� ��.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[SO_DEPTH_WRITE_OFF]);



	//----------------------------
	// ���ĺ��� ���� ��ü ���� : "��º��ձ� Output Merger" ���� ����. 
	//----------------------------
	//...
}




///////////////////////////////////////////////////////////////////////////////
//
//  ����/���ٽ� ���� ���°�ü �����ϱ� : ���� ����� 1ȸ ȣ��.
//
void CGraphicsSystem::DSStateRelease()
{
	for (int i = 0; i < SO_MAX_; i++)
		SAFE_RELEASE(m_DSState[i]);
}





/////////////////////////////////////////////////////////////////////////////
//
//  ��ġ-����ü��/����Ÿ��/����/���ٽ� ���� Ŭ���� 
//
void CGraphicsSystem::ClearBackBuffer()
{
	m_pDXDC->ClearRenderTargetView(m_pRTView, (float*)&m_BackColor);
	m_pDXDC->ClearDepthStencilView(m_pDSView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}



/////////////////////////////////////////////////////////////////////////////// 
//
// ������ ��� ��ȯ : ������ ����� ������ ���� ������ ���¸� ����.
//
#define CheckRMode(k, v) if((k)) m_RMode |= (v); else m_RMode &= ~(v);
void CGraphicsSystem::RenderModeUpdate()
{
	// ������ �ɼ� ����
	if (IsKeyUp(VK_SPACE))	CGraphicsSystem::m_bWireFrame ^= TRUE;
	if (IsKeyUp(VK_F4))		CGraphicsSystem::m_bCullBack ^= TRUE;
	//if (IsKeyUp(VK_F5))		CGraphicsSystem::m_bLightEnable ^= TRUE;
	if (IsKeyUp(VK_F5))		CGraphicsSystem::m_bZEnable ^= TRUE;
	if (IsKeyUp(VK_F6))		CGraphicsSystem::m_bZWrite ^= TRUE;


	// ���� ����.
	if (CGraphicsSystem::m_bWireFrame)	CGraphicsSystem::m_BackColor = COLOR(0.15f, 0.15f, 0.15f, 1.0f);
	else								CGraphicsSystem::m_BackColor = COLOR(0, 0.12f, 0.35f, 1);

	CheckRMode(m_bCullBack, RM_CULLBACK);
	CheckRMode(m_bWireFrame, RM_WIREFRAME);


	// ������ ��� ��ȯ : : ������ ����� ������ ���� ������ ���¸� ����.
	switch (m_RMode)
	{
	default:
	case RM_DEFAULT:
		m_pDXDC->RSSetState(m_RState[RS_DEFAULT]);
		break;
	case RM_WIREFRAME:
		m_pDXDC->RSSetState(m_RState[RS_WIREFRM]);
		break;
	case RM_CULLBACK:
		m_pDXDC->RSSetState(m_RState[RS_CULLBACK]);
		break;
	case RM_WIREFRAME | RM_CULLBACK:
		m_pDXDC->RSSetState(m_RState[RS_WIRECULLBACK]);
		break;
	}

	// ���� ���� ��� ��ȯ.	 
	if (m_bZEnable)
	{
		if (m_bZWrite)
			m_pDXDC->OMSetDepthStencilState(m_DSState[SO_DEPTH_ON], 0);			//���� ���� ���� (�⺻��)
		else  m_pDXDC->OMSetDepthStencilState(m_DSState[SO_DEPTH_WRITE_OFF], 0);	//���� ����  : Z-Test On + Z-Write Off.
	}
	else  m_pDXDC->OMSetDepthStencilState(m_DSState[SO_DEPTH_OFF], 0);	//���� ���� ��Ȱ��ȭ : Z-Test Off + Z-Write Off.

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