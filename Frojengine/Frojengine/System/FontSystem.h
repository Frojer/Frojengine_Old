#pragma once
#ifndef _FONT_SYSTEM_
#define _FONT_SYSTEM_

#include "..\Frojengine.h"

class CFontSystem
{
	SpriteBatch* m_pFontBatch;
	SpriteFont* m_pFont;

public:
	CFontSystem();
	CFontSystem(const CFontSystem& obj);
	~CFontSystem();

	bool Create(LPDEVICE pDevice, LPCWSTR fontFileName);
	void Release();

	void TextDraw(int x, int y, COLOR col, LPCWSTR msg, ...);
};

#endif