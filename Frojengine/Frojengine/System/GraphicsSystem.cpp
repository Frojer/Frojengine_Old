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
// 디바이스 개체 및 관련 장치 제거 : 종료시 1회 호출.  
// 기본 렌더링 개체들 제거시 생성된 역순으로 처리합니다.(권장)
//
void CGraphicsSystem::Release()
{
	ReleaseDX();
}




///////////////////////////////////////////////////////////////////////////// 
//
// DX 및 관련 객체 초기화 : 어플리케이션 시작시 1회 호출.
//
// 1.디바이스 생성 
// 2.스왑체인 및 렌더타겟 생성.
// 3.깊이/스텐실 버퍼 생성.
// 4.기타 렌더링 관련 장치 생성.
//
bool CGraphicsSystem::CreateDX(HWND hWnd)
{
	bool result;

	//----------------------------------------
	// 1단계 : 렌더링 장치 관련 필수 설정
	//----------------------------------------
	
	// D3D 렌더링 장치 Device 및 스왑체인 Swap Chain 생성. 
	result = CreateDeviceSwapChain(hWnd);

	if (!result)
	{
		return false;
	}

	// 장치-스왑체인의 렌더타겟(백버퍼) 획득
	result = CreateRenderTarget();
	
	if (!result)
	{
		return false;
	}

	// 깊이/스텐실 버퍼 생성.
	result = CreateDepthStencil();

	if (!result)
	{
		return false;
	}

	// 장치 출력병합기(Output Merger) 에 렌터링 타겟 및 깊이-스텐실 버퍼 등록.
	m_pDXDC->OMSetRenderTargets(
		1,				// 렌더타겟 개수.(max: D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
		&m_pRTView,		// 렌더타겟("백버퍼") 등록.	
		m_pDSView
	);

	// 뷰포트 설정
	//SetViewPort();


	//----------------------------------------
	// 2단계 : 기타 렌더링 관련 추가 설정.
	//----------------------------------------
	// 렌더링에 필요한 사용자 객체등을 생성/관리 합니다.
	// 카메라, 조명, 폰트, 셰이더 등등...
	StateObjectCreate();


	// 작업 완료, 장치 준비 완료.
	return true;
}




///////////////////////////////////////////////////////////////////////////// 
//
// 디바이스 개체 및 관련 장치 제거 : 종료시 1회 호출.  
// 기본 렌더링 개체들 제거시 생성된 역순으로 처리합니다.(권장)
//
void CGraphicsSystem::ReleaseDX()
{
	// 장치 상태 리셋 : 제거 전에 초기화를 해야 합니다. (메모리 누수 방지)
	if (m_pDXDC)
		m_pDXDC->ClearState();

	// 상태 객체 제거.
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
// D3D 렌더링 '장치(Device)' 및 스왑체인 생성. 
//
bool CGraphicsSystem::CreateDeviceSwapChain(HWND hWnd)
{
	HRESULT hr = S_OK;
	
	// D3D 기능 레벨 (Direct3D feature level) : DX11 미지원 HW 에서의 호환성 향상
	D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_0;		//DX11 대응.
	//D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_9_3;		//DX9.0c 대응.


	// 장치 (Device) 및 스왑체인(SwapChain) 정보 구성.
	// 스왑체인은 다중버퍼링 시스템을 말하며
	// 고전적인 '플립핑Flipping' 체인과 동일한 의미입니다.  
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Windowed = m_bWindowMode;							// 풀스크린 또는 창모드 선택.
	sd.OutputWindow = hWnd;						// 출력할 윈도우 핸들.
	sd.BufferCount = 1;							// 백버퍼 개수.
	//sd.BufferDesc = m_Mode;
	sd.BufferDesc.Width = m_Mode.Width;			// 해상도 결정.(백버퍼 크기)
	sd.BufferDesc.Height = m_Mode.Height;
	sd.BufferDesc.Format = m_Mode.Format;		// 백버퍼 색상규격 (A8R8G8B8) 창모드에서는 생략 가능 
	sd.BufferDesc.RefreshRate.Numerator = 0;    // 버퍼 갱신율.(수직동기화 VSync 활성화시 표준갱신율 적용 : 60hz)
	//sd.BufferDesc.RefreshRate.Numerator = 0;	// 버퍼 갱신율.(수직동기화 VSync Off)
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 용도 설정: '렌더타겟' 
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//sd.Flags = 0;
	sd.SampleDesc.Count = 1;					// AA 설정
	sd.SampleDesc.Quality = 0;
	
	
	
	// D3D 렌더링 '장치(Device)' 및 스왑체인 생성. 
	//
	hr = D3D11CreateDeviceAndSwapChain(
			NULL,							// 비디오 어뎁터 포인터 (기본장치는 NULL)
			D3D_DRIVER_TYPE_HARDWARE,		// HW 가속 
			NULL,							// SW Rasterizer DLL 핸들.  HW 가속시에는 NULL.
			0,								// 디바이스 생성 플래그.(기본값)
			//D3D11_CREATE_DEVICE_DEBUG,	// 디바이스 생성 플래그.(디버그)
			&featureLevels,					// (생성할) 디바이스 기능 레벨(Feature Level) 배열
			1,								// (생성할) 디바이스 기능 레벨(Feature Level) 배열 크기.
			D3D11_SDK_VERSION,				// DX SDK 버전.
			&sd,							// 디바이스 생성 상세 옵션.
			&m_pSwapChain,					// [출력] 스왑체인 인터페이스 얻기.
			&m_pDevice,						// [출력] 디바이스 인터페이스 얻기.
			NULL,							// [출력] (생성된) 디바이스 기능 레벨. 필요없다면 NULL 설정. 
			&m_pDXDC						// [출력] 디바이스 컨텍스트 얻기.
	);

	if (FAILED(hr))
	{
		WindowError(hr, L"디바이스 / 스왑체인 생성 실패.");
		return false;
	}
	
	return true;
}



/////////////////////////////////////////////////////////////////////////////
//
//  장치-스왑체인의 렌더타겟(백버퍼) 획득
//
//  리소스뷰(Resource View)  
//  DX11 의 리소스는 "공유" 목적으로 유연하게 설계되어 있습니다. 
//  (메모리 관리의 효율성 증대 및 다용도 활용이 가능..)
//  따라서 사용전 각 용도에 맞는 접근방식(인터페이스)를 획득(생성)해야 합니다.
//  이 작업을 "리소스 뷰 Resource View" 를 통해 처리합니다.
//                     
//  스왑체인에 연결(Bind)된 렌더링 출력버퍼 - "백버퍼 BackBuffer" 는 2D 기반 리소스(텍스처) 이며
//  "렌더타겟 RenderTarget" 용으로 리소스뷰를 생성, 해당 버퍼를 접근/운용 하겠습니다.
//
bool CGraphicsSystem::CreateRenderTarget()
{
	HRESULT hr = S_OK;

	// 벡버퍼 획득.
	ID3D11Texture2D *pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);		// 백버퍼 얻기, 2D 텍스쳐 행태, 화면출력용

	if (FAILED(hr))
		return false;

	//획득한 백버퍼에 렌더타겟 뷰 생성.(렌더타겟'형'으로 설정함)
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRTView);		// '렌더타겟뷰'를 생성.

	if (FAILED(hr))
	{
		WindowError(hr, L"백버퍼-렌더타겟 뷰 생성 실패.");
		return false;
	}

	//리소스 뷰 생성 후, 불필요한 DX 핸들은 해제해야 합니다.(메모리 누수 방지)
	SAFE_RELEASE(pBackBuffer);

	return true;
}





/////////////////////////////////////////////////////////////////////////////
//
// 깊이-스텐실버퍼 생성. : DX11 에서는 렌더링버퍼-렌더타겟뷰- 와 마찬가지로, 
//                       깊이-스텐실버퍼역시 직접 만들어야 합니다.
//                       디바이스에 등록도 역시 수동입니다.
//
bool CGraphicsSystem::CreateDepthStencil()
{
	HRESULT hr;

	// 깊이/스텐실 버퍼용 정보 구성.
	D3D11_TEXTURE2D_DESC   td;
	ZeroMemory(&td, sizeof(td));
	td.Width = m_Mode.Width;
	td.Height = m_Mode.Height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D32_FLOAT;			// 32BIT. 깊이 버퍼.
	td.SampleDesc.Count = 1;					// AA 설정 - RT 과 동일 규격 준수.
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// 깊이-스텐실 버퍼용으로 설정.
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	// 깊이 버퍼 생성.
	hr = m_pDevice->CreateTexture2D(&td, NULL, &m_pDS);
	if (FAILED(hr)) return false;



	// 깊이-스텐실버퍼용 리소스 뷰 정보 설정. 
	D3D11_DEPTH_STENCIL_VIEW_DESC  dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.Format = td.Format;
	dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // AA 없음.
	dd.Texture2D.MipSlice = 0;

	// 깊이-스텐실 버퍼 뷰 생성.
	hr = m_pDevice->CreateDepthStencilView(m_pDS, &dd, &m_pDSView);
	if (FAILED(hr))
	{
		WindowError(hr, L"깊이/스텐실 버퍼 뷰 생성 실패.");
		return false;
	}

	// 리소스 뷰 생성 후, 불필요한 DX 핸들은 해제해야 합니다.(메모리 누수 방지)
	// SAFE_RELEASE(pDS);

	return true;
}




/////////////////////////////////////////////////////////////////////////////
//
// 뷰포트 설정 :  DX11 에서는 기본처리 되지 않으며 사용자가 직접 설정해야합니다 
//			     (OpenGL 에서는 예전부터 이렇게 해 왔습니다.)
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
// 장치 렌더링 상태 객체 생성.
//
//----------------------------------------------------------------------------
// 상태 객체 State Objects (DX10/11)
// DX10 부터 구형 TnL 의 RenderState 가 제거되었습니다.
// 이를 대신하는 것이 상태객체 State Objects 인터페이스로, 렌더링 상태를 하나의 그룹으로 
// 묶고 렌더링시 디바이스에 설정합니다.  이를 통해 장치의 어려 상태 변화를 한번에 설정하여 
// 불필요한 연산부하(Overhead) 를 줄이고 보다 효과적인 렌더링을 가능케 합니다.
//
// 상태객체는 엔진 초기시 제작후 사용하기를 권장하며 종료시 제거(Release) 해야 합니다.
// 상태객체는 수정불가능(Immutable, 읽기전용) 개체 입니다.
// DX9 에서는 State-Block 이 이와 유사한 기능을 담당했었습니다.
//
// 상태 객체 인터페이스 중 '레스터라이즈 스테이지 Rasterize Stage' 상태 조절은 
// ID3D11RasterizerState 인터페이스를 통해 처리합니다.  
// 간단하게 렌더링 설정/기능 모듬 정도로 생각합시다.  자세한 것은 다음을 참조하십시요. 
// 링크1 : 상태 객체 https://msdn.microsoft.com/en-us/library/windows/desktop/bb205071(v=vs.85).aspx
// 링크2 : 깊이버퍼 상태 구성하기 https://msdn.microsoft.com/ko-kr/library/windows/desktop/bb205074(v=vs.85).aspx#Create_Depth_Stencil_State 
//----------------------------------------------------------------------------
//
void CGraphicsSystem::StateObjectCreate()
{

	//----------------------------
	// 레스터 상태 개체 생성 : "레스터라이즈 스테이지 Rasterize Stage" 상태 조절.
	//----------------------------
	RasterStateLoad();

	//----------------------------
	// 깊이/스텐실 상태 개체 생성.: "출력병합기 Output Merger" 상태 조절. 
	//----------------------------
	DSStateLoad();



	//----------------------------
	// 알파블렌딩 상태 개체 생성 : "출력병합기 Output Merger" 상태 조절. 
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
// 레스터 상태 객체 생성.
//
void CGraphicsSystem::RasterStateLoad()
{
	//기본 렌더링 상태 개체.
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;		// 삼각형 색상 채우기.(기본값)
	rd.CullMode = D3D11_CULL_NONE;		// 컬링 없음. (기본값은 컬링 Back)		
	rd.FrontCounterClockwise = false;   // 이하 기본값...
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = false;
	rd.AntialiasedLineEnable = false;
	//레스터라이져 객체 생성.
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_DEFAULT]);

	// 와이어 프레임 그리기. 
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	//레스터라이져 객체 생성.
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_WIREFRM]);

	// 컬링 On! "CCW"
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_CULLBACK]);

	//와이어 프레임 + 컬링 On! "CCW"
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	m_pDevice->CreateRasterizerState(&rd, &m_RState[RS_WIRECULLBACK]);
}




////////////////////////////////////////////////////////////////////////////// 
//
// 레스터 상태 객체 제거 : 어플리케이션 종료시 호출.
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
	// 레스터 상태 개체 생성 : "레스터라이즈 스테이지 Rasterize Stage" 상태 조절.
	//----------------------------
	//...


	//----------------------------
	// 깊이/스텐실 상태 개체 생성.: "출력병합기 Output Merger" 상태 조절. 
	//----------------------------
	//...	 
	D3D11_DEPTH_STENCIL_DESC  ds;
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS;
	ds.StencilEnable = FALSE;
	//ds.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	//ds.StnecilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	//...이하 기본값, 생략... 
	//...
	//첫번째 상태 객체 : Z-Test ON! (기본값)
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[SO_DEPTH_ON]);

	//두번째 상태 객체 : Z-Test OFF 상태.
	ds.DepthEnable = FALSE;
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[SO_DEPTH_OFF]);

	//세번째 상태 객체 : Z-Test On + Z-Write OFF.
	// Z-Test (ZEnable, DepthEnable) 이 꺼지면, Z-Write 역시 비활성화 됩니다.
	ds.DepthEnable = TRUE;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;		//깊이값 쓰기 끔.
	m_pDevice->CreateDepthStencilState(&ds, &m_DSState[SO_DEPTH_WRITE_OFF]);



	//----------------------------
	// 알파블렌딩 상태 개체 생성 : "출력병합기 Output Merger" 상태 조절. 
	//----------------------------
	//...
}




///////////////////////////////////////////////////////////////////////////////
//
//  깊이/스텐실 버퍼 상태객체 제거하기 : 엔진 종료시 1회 호출.
//
void CGraphicsSystem::DSStateRelease()
{
	for (int i = 0; i < SO_MAX_; i++)
		SAFE_RELEASE(m_DSState[i]);
}





/////////////////////////////////////////////////////////////////////////////
//
//  장치-스왑체인/렌더타겟/깊이/스텐실 버퍼 클리어 
//
void CGraphicsSystem::ClearBackBuffer()
{
	m_pDXDC->ClearRenderTargetView(m_pRTView, (float*)&m_BackColor);
	m_pDXDC->ClearDepthStencilView(m_pDSView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}



/////////////////////////////////////////////////////////////////////////////// 
//
// 렌더링 모드 전환 : 지정된 모드의 조합을 통해 렌더링 상태를 조절.
//
#define CheckRMode(k, v) if((k)) m_RMode |= (v); else m_RMode &= ~(v);
void CGraphicsSystem::RenderModeUpdate()
{
	// 렌더링 옵션 조절
	if (IsKeyUp(VK_SPACE))	CGraphicsSystem::m_bWireFrame ^= TRUE;
	if (IsKeyUp(VK_F4))		CGraphicsSystem::m_bCullBack ^= TRUE;
	//if (IsKeyUp(VK_F5))		CGraphicsSystem::m_bLightEnable ^= TRUE;
	if (IsKeyUp(VK_F5))		CGraphicsSystem::m_bZEnable ^= TRUE;
	if (IsKeyUp(VK_F6))		CGraphicsSystem::m_bZWrite ^= TRUE;


	// 배경색 설정.
	if (CGraphicsSystem::m_bWireFrame)	CGraphicsSystem::m_BackColor = COLOR(0.15f, 0.15f, 0.15f, 1.0f);
	else								CGraphicsSystem::m_BackColor = COLOR(0, 0.12f, 0.35f, 1);

	CheckRMode(m_bCullBack, RM_CULLBACK);
	CheckRMode(m_bWireFrame, RM_WIREFRAME);


	// 렌더링 모드 전환 : : 지정된 모드의 조합을 통해 렌더링 상태를 조절.
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

	// 깊이 연산 모드 전환.	 
	if (m_bZEnable)
	{
		if (m_bZWrite)
			m_pDXDC->OMSetDepthStencilState(m_DSState[SO_DEPTH_ON], 0);			//깊이 버퍼 동작 (기본값)
		else  m_pDXDC->OMSetDepthStencilState(m_DSState[SO_DEPTH_WRITE_OFF], 0);	//깊이 버퍼  : Z-Test On + Z-Write Off.
	}
	else  m_pDXDC->OMSetDepthStencilState(m_DSState[SO_DEPTH_OFF], 0);	//깊이 버퍼 비활성화 : Z-Test Off + Z-Write Off.

}



/////////////////////////////////////////////////////////////////////////////
//
//  장치-스왑체인 전환 : 장면 그리기 완료시 꼭 호출해야 함.
//
void CGraphicsSystem::Flip()
{
	m_pSwapChain->Present(0, 0);			// 화면 출력
}



////////////////////////////////////////////////
//
//   Get, Set 함수들 정의
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