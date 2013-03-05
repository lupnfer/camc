#include "StdAfx.h"
#include ".\dline.h"
#include "CAMCDoc.h"
#include "CmdImgDrawline.h"

CDLine* CDLine::_instance = NULL;
CDLine::CDLine(void)
{
}
CDLine* CDLine::Instance()
{
	if(_instance==NULL){
		_instance=new CDLine();
	}
	_instance->Initial();
	return _instance;
}
CDLine::~CDLine(void)
{
	
}
void CDLine::OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point)
{
	m_FirstPoint=point;
	m_SecondPoint=point;
	pView->m_CurrentDrawStatus=Draw_Status;
}
void CDLine::OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point)
{
	if(pView->m_CurrentDrawStatus==Draw_Status){
	
		pDC->SetROP2(R2_NOTXORPEN);
		this->Draw(pDC);
		m_SecondPoint=point;
		this->Draw(pDC);
	}
	
}
void CDLine::OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point)
{
	FCObjCanvas& objCanvas = pView->GetCanvas();
	POINT ptoffset = pView->GetCanvasOffset();
	RGBQUAD rgb;
	rgb.rgbBlue =GetBValue(m_PenColor);
	rgb.rgbGreen =GetGValue(m_PenColor);
	rgb.rgbRed = GetRValue(m_PenColor);
	rgb.rgbReserved = 0;
	
	objCanvas.MapRealPoint (m_FirstPoint);
	objCanvas.MapRealPoint (m_SecondPoint);
	objCanvas.MapRealPoint (ptoffset);
	objCanvas.ExecuteEffect(new CCmdImgDrawLine(ptoffset,255,m_FirstPoint,m_SecondPoint,rgb,m_PenWidth));
	
	pView->m_CurrentDrawStatus = DEFAULT;
}
void CDLine::Draw(CDC*pDC)
{
	CPen m_pen;
	m_pen.CreatePen(m_PenStyle,m_PenWidth,m_PenColor);
	CPen* pen=(CPen*)pDC->SelectObject(&m_pen);
	COLORREF OldColor=pDC->SetBkColor(m_BackgroundColor);
	int OldBkMode=pDC->SetBkMode(m_BkMode);
	pDC->MoveTo(m_FirstPoint);
	pDC->LineTo(m_SecondPoint);
	pDC->SelectObject(pen);
	pDC->SetBkMode(m_BkMode);
	pDC->SetBkColor(OldColor);

}
void CDLine::Initial()
{
	m_BkMode=TRANSPARENT;
	m_PenColor=RGB(0,0,0);
	m_BackgroundColor=RGB(255,255,255);
	m_PenWidth=1;
	m_PenStyle=PS_SOLID;
	m_DrawingMode=R2_COPYPEN;
	m_FirstPoint = 0;
	m_SecondPoint = 0 ;
}