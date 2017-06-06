#include "FontSystem.h"

CFontSystem::CFontSystem()
{
	m_pFontBatch = nullptr;
	m_pFont = nullptr;
}

CFontSystem::CFontSystem(const CFontSystem& obj)
{

}

CFontSystem::~CFontSystem()
{

}


bool CFontSystem::Create(LPDEVICE pDevice, LPCWSTR fontFileName)
{
	//��ġ ��� ȹ��.
	LPDXDC pDXDC = nullptr;
	pDevice->GetImmediateContext(&pDXDC);


	//Sprite Batch ��ü ����.
	m_pFontBatch = new SpriteBatch(pDXDC);

	//DirectX Toolkit : Sprite Font ��ü ����.
	//TCHAR* filename = L"../Font/����9k.sfont";	//ASCII 0 ~ 255 + Ư������'��' + Unicode �ѱ� �ϼ��� �� 11,440 ����, ũ��:9		
	
	try {
		m_pFont = new SpriteFont(pDevice, fontFileName);
		m_pFont->SetLineSpacing(14.0f);				//��Ʈ9 ����, �ٰ��� ����. '���߶��� ��½� ����� ������'
		m_pFont->SetDefaultCharacter('_');			//��� ���ڰ� �̰˻��� ��� ����� Ű��.
	}
	catch (std::exception& e)
	{
		//ynError(0, L"��Ʈ ���� ���� : File=%s", filename);
		TCHAR msg[1024] = L"";
		::mbstowcs(msg, e.what(), strlen(e.what()));
		WindowError(0, L"��Ʈ ���� ���� : %s \n File=%s", msg, fontFileName);

		SAFE_RELEASE(pDXDC);

		return true;
	}

	//�����, ��ġ��� ����.
	SAFE_RELEASE(pDXDC);

	return true;
}


void CFontSystem::Release()
{
	SAFE_DELETE(m_pFontBatch);
	SAFE_DELETE(m_pFont);
}



void CFontSystem::TextDraw(int x, int y, COLOR col, LPCWSTR msg, ...)
{
	const DWORD _size = 2048;

	TCHAR buff[_size] = L"";
	va_list vl;
	va_start(vl, msg);
	_vstprintf(buff, msg, vl);
	va_end(vl);

	m_pFontBatch->Begin();
	//g_pFontBatch->Begin(SpriteSortMode_Deferred, nullptr, nullptr, ypStateObj[YNSO_DEPTH_ON_]);		//���̿��� �߰�.
	m_pFont->DrawString(m_pFontBatch, buff, Vector2((float)x, (float)y), Vector4(col));					//���� ũ��.
																										//g_pFont->DrawString(ypFontBatch, buff, Vector2((float)x, (float)y), Vector4(col), 0, Vector4::Zero, 0.8f);	//80% ��� ���.
	m_pFontBatch->End();
}