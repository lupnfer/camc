#include "StdAfx.h"
#include ".\unit.h"


void CUnit::Initial()
{
	m_BkMode=TRANSPARENT;
	m_PenColor=RGB(0,0,0);
	m_BackgroundColor=RGB(255,255,255);
	m_PenWidth=1;
	m_PenStyle=PS_SOLID;
	m_DrawingMode=R2_COPYPEN;
	m_DrawStatus=Draw_Status;
}
CUnit::CUnit(void)
{
	Initial();
}

CUnit::~CUnit(void)
{
}

CUnit::CUnit(CUnit&unit)
{
	m_PenColor=unit.m_PenColor;
	m_PenWidth=unit.m_PenWidth;
	m_PenStyle=unit.m_PenStyle;
}

CUnit CUnit::operator =(CUnit&unit)
{
	m_PenColor=unit.m_PenColor;
	m_PenWidth=unit.m_PenWidth;
	m_PenStyle=unit.m_PenStyle;
	return *this;
}

void CUnit::OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point)
{
	pView->m_CurrentDrawStatus=Draw_Status;
}
void CUnit::OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point)
{
	
}
void CUnit::OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point)
{	
	pView->m_CurrentDrawStatus = DEFAULT;
	//RECT rcSave=GetRect();
	pView->InvalidateRect(&m_rect);
}
void CUnit::Draw(CDC*pDC)
{
	
}
