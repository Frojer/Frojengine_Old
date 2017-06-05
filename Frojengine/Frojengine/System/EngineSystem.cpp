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
			// ���� ����
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
// InitWindow  : ������ ���� �Լ�.
//
// width  : ������ ������ ���� ũ�� (pixel)
// height : ������ ������ ���� ũ�� (pixel)
//
// �Է¹��� ũ�⿡ ���߾� ������ ������ �����ϱ� ���ؼ� 
// Ŭ���̾�Ʈ ������ ���� ������ ��ü ũ�⸦ �������մϴ�.
//
bool CEngineSystem::InitWindow()
{
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0, 0,
		::GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		m_ClassName, NULL
	};

	RegisterClassEx(&wc);

	//������ ����.
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

	// ������ �ڵ� �������� ����.
	m_hWnd = hWnd;


	// Ŭ���̾�Ʈ ���� ũ�� ������ : �ʼ�. ��������
	ResizeWindow();

	// ���콺 Ŀ�� : �⺻ȭ��ǥ�� ����.
	SetCursor(LoadCursor(NULL, IDC_ARROW));

	return true;
}



//////////////////////////////////////////////////////////////////////////////
//
// void ResizeWindow(UINT newWidth, UINT newHeight)
//
// â����� ��� �ػ� ����� ���߾� ��ü ������ ũ�⸦ �������մϴ�. 
// ex)
//'Ŭ���� ��Ʈ'������ 640 * 480 �� ���߾�  ��ü �������� ũ�⸦ 
// �ٽ� �����մϴ�. --> �׷��� '���� �̹���'�� ��� ȭ�鿡 ���δ�.
//
// ��  �� :  Ŭ���̾�Ʈ ������ ũ�� - NewWidth, NewHeight
// ���ϰ� :  ����. 
//
void CEngineSystem::ResizeWindow()
{
	//���� �������� ��Ÿ�� ���ϱ�.
	RECT oldrc;
	DWORD sytle = (DWORD) ::GetWindowLong(m_hWnd, GWL_STYLE);
	DWORD exstyle = (DWORD) ::GetWindowLong(m_hWnd, GWL_EXSTYLE);

	//���� ������ '��ü' ũ�� (��ũ����ǥ) �� ��´�.
	::GetWindowRect(m_hWnd, &oldrc);

	//���� ������ �������� 'Ŭ���̾�Ʈ' ���� ũ�� ����ϱ�.
	RECT newrc;
	newrc.left = 0;
	newrc.top = 0;
	newrc.right = m_Width;
	newrc.bottom = m_Height;

	//newrc ��ŭ�� Ŭ���̾�Ʈ ������ �����ϴ� ������ '��ü' ũ�⸦ ���մϴ�.
	//���� '�޴�' �� ���ٴ� �����Ͽ� ó���ǰ� ����.
	//���� ����� newrc �� �ٽ� ���´�. ( ��ũ�� ��ǥ )
	//exstyle &= ~WS_EX_TOPMOST;
	::AdjustWindowRectEx(&newrc, sytle, NULL, exstyle);


	//������ �������� �ʺ�� ���� ���մϴ�. 
	int newWidth = (newrc.right - newrc.left);
	int newHeight = (newrc.bottom - newrc.top);


	//���ο� ũ�⸦ �����쿡 �����մϴ�.
	::SetWindowPos(m_hWnd, HWND_NOTOPMOST,
		oldrc.left, oldrc.top,
		newWidth, newHeight, SWP_SHOWWINDOW);
}


/////////////////////////////////////////////////////////////////////////////
// 
// int MessagePump () : ������ �޼��� ó�� �Լ�.
// 
// ��������(OS)�κ��� �� ���ø����̼����� Ư���� �޼����� �������  
// �ʴ´ٸ�  ������ ���� �ϵ��� �մϴ�.                               
// 
bool CEngineSystem::MessagePump()
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(msg));

	while (1)
	{
		// �޼��� ť�� �޼����� ������...ó���ϰ�..
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)			// ������ ���Ḧ ���� ����.
			{
				//m_bEnd = true;
				return false;
			}

			// ������ �޼��� ó��.
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else  // Ư���� �޼����� ���ٸ� ������ ����(������)�� �����մϴ�..
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
	// ����/�ý��� ����.
	//------------------------------- 
	
	GetEngineTime();




	// ��� ���� 
	//
	// ���ΰ�, ����, ������ �̵�, �浹�˻� ���..
	// ...

	// ���� ����, �̺�Ʈ, ����, �������..
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
	//static float evfps = 0;		//��� ������.
	UINT nowtime = GetTickCount();
	++frmcnt;

	TCHAR msg[80];
	int time = nowtime - oldtime;
	if (time >= 999) // 0~999 �и�������.. 1~1000�� �ƴ�
	{
		oldtime = nowtime;

		//1�ʰ� ������ ������ ���� ���մϴ�..
		frm = frmcnt;	frmcnt = 0;

		//�ʴ� ������ ���� ����մϴ�.
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
//  ����. 
//
void ShowInfo(HWND g_hWnd)
{
	int x = 100;
	int y = 100;
	HDC hdc = GetDC(g_hWnd);
	TCHAR* msg = L"DX �� ����ϱ� ���� 3D ������ �⺻ �����ӿ�ũ �Դϴ�.";
	TextOut(hdc, x, y += 20, msg, (int)_tcslen(msg));
	msg = L"�⺻ �����ӿ�ũ�� ������� �ܰ躰�� Ʃ�丮���� ������ ���Դϴ�.";
	TextOut(hdc, x, y += 20, msg, (int)_tcslen(msg));
	y += 20;
	msg = L"Idle Time �� ���߾� ��������� �޼����� �����Ÿ��ϴ�.";
	TextOut(hdc, x, y += 20, msg, (int)_tcslen(msg));

	ReleaseDC(g_hWnd, hdc);

}



void CEngineSystem::Render()
{


	//-------------------------------
	//  ��� �׸��� ����.. 
	//-------------------------------
	//  ����Ÿ��(�����) �����.. 
	m_pGraphics->ClearBackBuffer();


	//��ü ������ : ���ΰ�, ����, ����.. 
	//...


	//���� �� ��Ÿ ������ ���� ���.
	//�����Ӽ� ǥ��.
	PutFPS(1, 1, m_hWnd);

	//���� ���.
	ShowInfo(m_hWnd);


	//-------------------------------
	// ��� �׸��� ����.
	//------------------------------- 
	m_pGraphics->Flip();
}



//////////////////////////////////////////////////////////////////////////////
//
// MsgProc :  ������ �޼��� ���ν���.
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
// ���� �޼��� ó��
//
void WindowErrorW(TCHAR* file, UINT line, TCHAR* func, BOOL bMBox, HRESULT hr, TCHAR* msg, ...)
{
	TCHAR msgva[1024] = L"";
	va_list vl;
	va_start(vl, msg);
	_vstprintf(msgva, msg, vl);
	va_end(vl);


	// HRESULT ���� ���� �޼��� ���.
	TCHAR herr[1024] = L"";
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		herr, 1024, NULL);

	// ���� �޼��� ���̱�.
	TCHAR errmsg[2048] = L"";
	_stprintf(errmsg, L"%s \n�����ڵ�(0x%X) : %s \nFile = %s (%d)\nFunc = %s\n\n",
		msgva, hr, herr,
		file, line, func);


	// (����� ��) VS ���â���� ���..
	OutputDebugString(L"\n");
	OutputDebugString(errmsg);

	// �α����Ϸ� ���.
	// ...

	// �޼��� â ���..
	if (bMBox)
	{
		MessageBox(NULL, errmsg, L"Window::Error", MB_OK | MB_ICONERROR);
		//GetLastError(hr);
	}
}

////////////////////////////////////////////////////////////////////////////// 
//
// ���� �޼��� ó�� : ���̴� ���� ó����.
// 
void WindowErrorW(BOOL bMBox, TCHAR* msg, HRESULT hr, ID3DBlob* pBlob, TCHAR* filename, char* EntryPoint, char* ShaderModel)
{

	/*//�����Ű����� ó��.
	TCHAR msgva[2048] = L"";
	va_list vl;
	va_start(vl, msg);
	_vstprintf(msgva, msg, vl);
	va_end(vl);
	*/
	//�Ķ����, �����ڵ�� ��ȯ.
	TCHAR func[80] = L"";
	::mbstowcs(func, EntryPoint, strlen(EntryPoint));
	TCHAR sm[20] = L"";
	::mbstowcs(sm, ShaderModel, strlen(ShaderModel));


	//���̴� ���� �޼��� �б�.
	TCHAR errw[4096] = L"";
	::mbstowcs(errw, (char*)pBlob->GetBufferPointer(), pBlob->GetBufferSize());


	//HRESULT ���� ���� �޼��� ��� 
	//�ý������� ���� ��� ���̴� �����޼����� ����Ȯ�ϹǷ� ����.
	TCHAR herr[1024] = L"�Ʒ��� ������ Ȯ���Ͻʽÿ�.";
	/*FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr,
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	herr, 1024, NULL);
	*/


	//���� �޼��� ���̱�.
	TCHAR errmsg[1024];
	_stprintf(errmsg, L"%s \nFile=%s  Entry=%s  Target=%s  \n�����ڵ�(0x%08X) : %s \n\n%s",
		msg, filename, func, sm,
		hr, herr, errw);


	//(����� ��) VS ���â���� ���..
	OutputDebugString(L"\n");
	OutputDebugString(errmsg);
	//OutputDebugString(errw);


	//�α����Ϸ� ���.
	//...


	//�޼��� â ���..
	if (bMBox)
	{
		MessageBox(NULL, errmsg, L"Yena::Error", MB_OK | MB_ICONERROR);
		//MessageBox(NULL, errw, L"Yena::Error", MB_OK | MB_ICONERROR);
	}
}