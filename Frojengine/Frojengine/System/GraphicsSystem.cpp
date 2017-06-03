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
// 디바이스 개체 및 관련 장치 제거 : 종료시 1회 호출.  
// 기본 렌더링 개체들 제거시 생성된 역순으로 처리합니다.(권장)
//
void CGraphicsSystem::Release()
{
	// 장치 상태 리셋 : 제거 전에 초기화를 해야 합니다. (메모리 누수 방지)
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

	// 장치 출력병합기(Output Merger) 에 렌터링 타겟 및 깊이-스텐실 버퍼 등록.
	m_pDXDC->OMSetRenderTargets(
		1,				// 렌더타겟 개수.(max: D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
		&m_pRTView,		// 렌더타겟("백버퍼") 등록.	
		nullptr
	);

	// 뷰포트 설정
	SetViewPort();


	//----------------------------------------
	// 2단계 : 기타 렌더링 관련 추가 설정.
	//----------------------------------------
	// 렌더링에 필요한 사용자 객체등을 생성/관리 합니다.
	// 카메라, 조명, 폰트, 셰이더 등등...



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
	sd.Windowed = TRUE;							// 풀스크린 또는 창모드 선택.
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
	ID3D11RenderTargetView* pRTView;
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRTView);		// '렌더타겟뷰'를 생성.

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




/////////////////////////////////////////////////////////////////////////////
//
//  장치-스왑체인/렌더타겟/깊이/스텐실 버퍼 클리어 
//
void CGraphicsSystem::ClearBackBuffer()
{
	m_pDXDC->ClearRenderTargetView(m_pRTView, (float*)&m_BackColor);
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