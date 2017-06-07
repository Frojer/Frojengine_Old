#include "..\Frojengine.h"

CLine::CLine()
{

}

CLine::CLine(const CLine& obj)
{

}

CLine::~CLine()
{

}


bool CLine::Create(LPDEVICE pDevice, const LineVertexFormat& startPos, const LineVertexFormat& endPos)
{
	m_pDevice = pDevice;
	m_pDevice->GetImmediateContext(&m_pDXDC);

	m_startPos = startPos;
	m_endPos = endPos;

	return true;
}


void CLine::Release()
{

}