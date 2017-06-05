#include "EngineSystem.h"

bool CEngineSystem::m_bEnd = true;

CEngineSystem::CEngineSystem()
{
	m_hWnd = nullptr;
	m_bEnd = true;
	m_pGraphics = nullptr;
	m_ClassName = nullptr;
	m_WindowName = nullptr;

	m_CurScene = nullptr;
	m_ChangeScene = nullptr;
}


CEngineSystem::CEngineSystem(const CEngineSystem& obj)
{

}


CEngineSystem::~CEngineSystem()
{

}


bool CEngineSystem::Create(LPCWSTR className, LPCWSTR windowName, int width, int height)
{
	bool result;

	m_bEnd = false;

	m_ClassName = className;
	m_WindowName = windowName;

	m_Width = width;
	m_Height = height;

	result = InitWindow();

	if (!result)
	{
		return false;
	}

	m_pGraphics = new CGraphicsSystem;

	if (m_pGraphics == nullptr)
	{
		return false;
	}

	result = m_pGraphics->Create(m_hWnd, m_Width, m_Height);

	if (!result)
	{
		return false;
	}

	return true;
}


void CEngineSystem::Release()
{
	SAFE_RELEASE(m_pGraphics);
}


void CEngineSystem::Run()
{
	while (!m_bEnd)
	{
		bool result;
		result = MessagePump();

		if (result)
		{
			// 게임 루프
			Update();
			Render();
		}
	}
}


void CEngineSystem::GetWindowSize(int& oWidth, int& oHeight)
{
	oWidth = m_Width;
	oHeight = m_Height;
}



void CEngineSystem::PrintText(int x, int y, LPCWSTR string, int size)
{
	HDC hdc = GetDC(m_hWnd);
	TextOut(hdc, x, y, string, size);
	ReleaseDC(m_hWnd, hdc);
}



/////////////////////////////////////////////////////////////////////////////
//
// InitWindow  : 윈도우 생성 함수.
//
// width  : 생성할 윈도우 가로 크기 (pixel)
// height : 생성할 윈도우 세로 크기 (pixel)
//
// 입력받은 크기에 맞추어 렌더링 영역을 생성하기 위해서 
// 클라이언트 영역을 포함 윈도우 전체 크기를 재조정합니다.
//
bool CEngineSystem::InitWindow()
{
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0, 0,
		::GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		m_ClassName, NULL
	};

	RegisterClassEx(&wc);

	//윈도우 생성.
	HWND hWnd = ::CreateWindow(m_ClassName, m_WindowName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		//WS_OVERLAPPEDWINDOW, 
		0, 0,
		m_Width, m_Height,
		GetDesktopWindow(), NULL,
		wc.hInstance, NULL
	);
	
	if (hWnd == NULL)
		return false;


	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd);

	// 윈도우 핸들 전역변수 복사.
	m_hWnd = hWnd;


	// 클라이언트 영역 크기 재조정 : 필수. 삭제금지
	ResizeWindow();

	// 마우스 커서 : 기본화살표로 설정.
	SetCursor(LoadCursor(NULL, IDC_ARROW));

	return true;
}



//////////////////////////////////////////////////////////////////////////////
//
// void ResizeWindow(UINT newWidth, UINT newHeight)
//
// 창모드의 경우 해상도 사이즈에 맞추어 전체 윈도우 크기를 재조정합니다. 
// ex)
//'클라이 언트'영역을 640 * 480 에 맞추어  전체 윈도우의 크기를 
// 다시 보정합니다. --> 그래야 '게임 이미지'가 모두 화면에 보인다.
//
// 인  자 :  클라이언트 영역의 크기 - NewWidth, NewHeight
// 리턴값 :  없음. 
//
void CEngineSystem::ResizeWindow()
{
	//현재 윈도우의 스타일 구하기.
	RECT oldrc;
	DWORD sytle = (DWORD) ::GetWindowLong(m_hWnd, GWL_STYLE);
	DWORD exstyle = (DWORD) ::GetWindowLong(m_hWnd, GWL_EXSTYLE);

	//현재 윈도우 '전체' 크기 (스크린좌표) 를 얻는다.
	::GetWindowRect(m_hWnd, &oldrc);

	//새로 생성될 윈도우의 '클라이언트' 영역 크기 계산하기.
	RECT newrc;
	newrc.left = 0;
	newrc.top = 0;
	newrc.right = m_Width;
	newrc.bottom = m_Height;

	//newrc 만큼의 클라이언트 영역을 포함하는 윈도우 '전체' 크기를 구합니다.
	//현재 '메뉴' 는 없다는 가정하에 처리되고 있음.
	//계산된 결과는 newrc 에 다시 들어온다. ( 스크린 좌표 )
	//exstyle &= ~WS_EX_TOPMOST;
	::AdjustWindowRectEx(&newrc, sytle, NULL, exstyle);


	//보정된 윈도우의 너비와 폭을 구합니다. 
	int newWidth = (newrc.right - newrc.left);
	int newHeight = (newrc.bottom - newrc.top);


	//새로운 크기를 윈도우에 설정합니다.
	::SetWindowPos(m_hWnd, HWND_NOTOPMOST,
		oldrc.left, oldrc.top,
		newWidth, newHeight, SWP_SHOWWINDOW);
}


/////////////////////////////////////////////////////////////////////////////
// 
// int MessagePump () : 윈도우 메세지 처리 함수.
// 
// 윈도우즈(OS)로부터 이 어플리케이션으로 특별히 메세지가 날라오지  
// 않는다면  게임을 진행 하도록 합니다.                               
// 
bool CEngineSystem::MessagePump()
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(msg));

	while (1)
	{
		// 메세지 큐에 메세지가 있으면...처리하고..
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)			// 윈도우 종료를 위해 리턴.
			{
				//m_bEnd = true;
				return false;
			}

			// 나머지 메세지 처리.
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else  // 특별한 메세지가 없다면 리턴후 게임(렌더링)을 진행합니다..
		{
			return true;
		}
	}

	return false;
}



void CEngineSystem::GetEngineTime()
{
	static int oldtime = GetTickCount();
	int nowtime = GetTickCount();
	m_deltaTime = (nowtime - oldtime) * 0.001f;
	oldtime = nowtime;
}



void CEngineSystem::Update()
{
	//-------------------------------
	// 엔진/시스템 갱신.
	//------------------------------- 
	
	GetEngineTime();




	// 장면 갱신 
	//
	// 주인공, 몬스터, 오브제 이동, 충돌검사 등등..
	// ...

	// 게임 로직, 이벤트, 층돌, 점수계산..
	// ...	 
}


/////////////////////////////////////////////////////////////////////////////
//
void PutFPS(int x, int y, HWND g_hWnd)
{
	static int frm = 0;
	static UINT oldtime = GetTickCount();
	static UINT frmcnt = 0;
	static float  fps = 0.0f;
	//static float evfps = 0;		//평균 프레임.
	UINT nowtime = GetTickCount();
	++frmcnt;

	TCHAR msg[80];
	int time = nowtime - oldtime;
	if (time >= 999) // 0~999 밀리세컨드.. 1~1000이 아님
	{
		oldtime = nowtime;

		//1초간 증가된 프레임 수를 구합니다..
		frm = frmcnt;	frmcnt = 0;

		//초당 프래임 수를 계산합니다.
		fps = (float)(frm * 1000) / (float)time;
	}

	//sprintf(msg, "fps=%.1f/%d      ", fps, time); 
	_stprintf(msg, L"fps=%d/%d      ", frm, time);

	HDC hdc = GetDC(g_hWnd);
	TextOut(hdc, x, y, msg, (int)_tcslen(msg));
	ReleaseDC(g_hWnd, hdc);
}






/////////////////////////////////////////////////////////////////////////////// 
//
//  도움말. 
//
void ShowInfo(HWND g_hWnd)
{
	int x = 100;
	int y = 100;
	HDC hdc = GetDC(g_hWnd);
	TCHAR* msg = L"DX 를 사용하기 위한 3D 렌더링 기본 프레임워크 입니다.";
	TextOut(hdc, x, y += 20, msg, (int)_tcslen(msg));
	msg = L"기본 프레임워크를 기반으로 단계별로 튜토리얼을 진행할 것입니다.";
	TextOut(hdc, x, y += 20, msg, (int)_tcslen(msg));
	y += 20;
	msg = L"Idle Time 에 맞추어 출력중으로 메세지가 깜빡거립니다.";
	TextOut(hdc, x, y += 20, msg, (int)_tcslen(msg));

	ReleaseDC(g_hWnd, hdc);

}



void CEngineSystem::Render()
{


	//-------------------------------
	//  장면 그리기 시작.. 
	//-------------------------------
	//  렌더타겟(백버퍼) 지우기.. 
	m_pGraphics->ClearBackBuffer();


	//개체 렌더링 : 주인공, 몬스터, 지형.. 
	//...


	//도움말 및 기타 렌더링 정보 출력.
	//프레임수 표시.
	PutFPS(1, 1, m_hWnd);

	//도움말 출력.
	ShowInfo(m_hWnd);


	//-------------------------------
	// 장면 그리기 종료.
	//------------------------------- 
	m_pGraphics->Flip();
}



//////////////////////////////////////////////////////////////////////////////
//
// MsgProc :  윈도우 메세지 프로시져.
// 
LRESULT CALLBACK CEngineSystem::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			break;
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		m_bEnd = true;
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}




////////////////////////////////////////////////////////////////////////////// 
//
// 에러 메세지 처리
//
void WindowErrorW(TCHAR* file, UINT line, TCHAR* func, BOOL bMBox, HRESULT hr, TCHAR* msg, ...)
{
	TCHAR msgva[1024] = L"";
	va_list vl;
	va_start(vl, msg);
	_vstprintf(msgva, msg, vl);
	va_end(vl);


	// HRESULT 에서 에러 메세지 얻기.
	TCHAR herr[1024] = L"";
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		herr, 1024, NULL);

	// 에러 메세지 붙이기.
	TCHAR errmsg[2048] = L"";
	_stprintf(errmsg, L"%s \n에러코드(0x%X) : %s \nFile = %s (%d)\nFunc = %s\n\n",
		msgva, hr, herr,
		file, line, func);


	// (디버깅 중) VS 출력창으로 출력..
	OutputDebugString(L"\n");
	OutputDebugString(errmsg);

	// 로그파일로 출력.
	// ...

	// 메세지 창 출력..
	if (bMBox)
	{
		MessageBox(NULL, errmsg, L"Window::Error", MB_OK | MB_ICONERROR);
		//GetLastError(hr);
	}
}

////////////////////////////////////////////////////////////////////////////// 
//
// 에러 메세지 처리 : 셰이더 에러 처리용.
// 
void WindowErrorW(BOOL bMBox, TCHAR* msg, HRESULT hr, ID3DBlob* pBlob, TCHAR* filename, char* EntryPoint, char* ShaderModel)
{

	/*//가변매개변수 처리.
	TCHAR msgva[2048] = L"";
	va_list vl;
	va_start(vl, msg);
	_vstprintf(msgva, msg, vl);
	va_end(vl);
	*/
	//파라미터, 유니코드로 전환.
	TCHAR func[80] = L"";
	::mbstowcs(func, EntryPoint, strlen(EntryPoint));
	TCHAR sm[20] = L"";
	::mbstowcs(sm, ShaderModel, strlen(ShaderModel));


	//셰이더 오류 메세지 읽기.
	TCHAR errw[4096] = L"";
	::mbstowcs(errw, (char*)pBlob->GetBufferPointer(), pBlob->GetBufferSize());


	//HRESULT 에서 에러 메세지 얻기 
	//시스템으로 부터 얻는 셰이더 오류메세지는 부정확하므로 생략.
	TCHAR herr[1024] = L"아래의 오류를 확인하십시요.";
	/*FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr,
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	herr, 1024, NULL);
	*/


	//에러 메세지 붙이기.
	TCHAR errmsg[1024];
	_stprintf(errmsg, L"%s \nFile=%s  Entry=%s  Target=%s  \n에러코드(0x%08X) : %s \n\n%s",
		msg, filename, func, sm,
		hr, herr, errw);


	//(디버깅 중) VS 출력창으로 출력..
	OutputDebugString(L"\n");
	OutputDebugString(errmsg);
	//OutputDebugString(errw);


	//로그파일로 출력.
	//...


	//메세지 창 출력..
	if (bMBox)
	{
		MessageBox(NULL, errmsg, L"Yena::Error", MB_OK | MB_ICONERROR);
		//MessageBox(NULL, errw, L"Yena::Error", MB_OK | MB_ICONERROR);
	}
}