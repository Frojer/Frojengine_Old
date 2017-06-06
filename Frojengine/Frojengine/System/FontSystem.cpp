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
	//장치 목록 획득.
	LPDXDC pDXDC = nullptr;
	pDevice->GetImmediateContext(&pDXDC);


	//Sprite Batch 개체 생성.
	m_pFontBatch = new SpriteBatch(pDXDC);

	//DirectX Toolkit : Sprite Font 객체 생성.
	//TCHAR* filename = L"../Font/굴림9k.sfont";	//ASCII 0 ~ 255 + 특수문자'■' + Unicode 한글 완성형 총 11,440 글자, 크기:9		
	
	try {
		m_pFont = new SpriteFont(pDevice, fontFileName);
		m_pFont->SetLineSpacing(14.0f);				//폰트9 기준, 줄간격 설정. '다중라인 출력시 흐려짐 방지용'
		m_pFont->SetDefaultCharacter('_');			//출력 글자값 미검색시 대신 출력할 키값.
	}
	catch (std::exception& e)
	{
		//ynError(0, L"폰트 생성 실패 : File=%s", filename);
		TCHAR msg[1024] = L"";
		::mbstowcs(msg, e.what(), strlen(e.what()));
		WindowError(0, L"폰트 생성 실패 : %s \n File=%s", msg, fontFileName);

		SAFE_RELEASE(pDXDC);

		return true;
	}

	//사용후, 장치목록 해제.
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
	//g_pFontBatch->Begin(SpriteSortMode_Deferred, nullptr, nullptr, ypStateObj[YNSO_DEPTH_ON_]);		//깊이연산 추가.
	m_pFont->DrawString(m_pFontBatch, buff, Vector2((float)x, (float)y), Vector4(col));					//원본 크기.
																										//g_pFont->DrawString(ypFontBatch, buff, Vector2((float)x, (float)y), Vector4(col), 0, Vector4::Zero, 0.8f);	//80% 축소 출력.
	m_pFontBatch->End();
}