#include "StdAfx.h"
#include ".\curve.h"
#include "CmdImgDrawline.h"
#include "cmdimgdrawanyline.h"
#include "CAMCDoc.h"

CCurve* CCurve::_instance=NULL;
CCurve::CCurve(void)
{
}

CCurve::~CCurve(void)
{
}
CCurve* CCurve::Instance()
{
	if(_instance==NULL){
		_instance=new CCurve();
	}
	_instance->Initial();
	return _instance;
}
void CCurve::OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point)
{
	m_FirstPoint=point;
	pView->m_CurrentDrawStatus=Draw_Status;

}
void CCurve::OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point)
{
	FCObjCanvas& objCanvas = pView->GetCanvas();
	RGBQUAD rgb;
	rgb.rgbBlue =GetBValue(m_PenColor);
	rgb.rgbGreen =GetGValue(m_PenColor);
	rgb.rgbRed = GetRValue(m_PenColor);
	rgb.rgbReserved = 0;	
	POINT offset=pView->GetCanvasOffset();
	
	for(int i=0;i<m_pointList.GetSize();i++)
	{
		objCanvas.MapRealPoint (m_pointList.GetAt(i));
	}
	objCanvas.MapRealPoint (offset);
	
	objCanvas.ExecuteEffect(new CCmdImgDrawAnyLine(m_pointList,offset,255,rgb,m_PenWidth,m_PenStyle));
	pView->m_CurrentDrawStatus = DEFAULT;

}
void CCurve::DrawMask(CDC*pDC,CPoint first,CPoint second)
{
	CPen m_pen;
	m_pen.CreatePen(m_PenStyle,m_PenWidth,m_PenColor);
	CPen* pen=(CPen*)pDC->SelectObject(&m_pen);
	pDC->MoveTo(first);
	pDC->LineTo(second);
	pDC->SelectObject(pen);
}

void CCurve::OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point)
{
	
	
}
void CCurve::Draw(CDC*pDC)
{
	CPen m_pen;	
	if(m_pointList.IsEmpty())
		return;
	m_pen.CreatePen(m_PenStyle,m_PenWidth,m_PenColor);
	CPen* pen=(CPen*)pDC->SelectObject(&m_pen);
	int oldDrawingMode=pDC->SetROP2(m_DrawingMode);
	m_FirstPoint=m_pointList.GetAt(0);
	pDC->MoveTo(m_FirstPoint);
	int i=0;
	for(;i<m_pointList.GetSize();i++)
	{
		pDC->LineTo(m_pointList.GetAt(i));
		pDC->LineTo(m_pointList.GetAt(i));
	}
	pDC->LineTo(m_pointList.GetAt(i-1));
	pDC->SelectObject(pen);
	pDC->SetROP2(oldDrawingMode);  

}
void CCurve::Initial()
{
	m_BkMode=TRANSPARENT;
	m_PenColor=RGB(0,0,0);
	m_BackgroundColor=RGB(255,255,255);
	m_PenWidth=2;
	m_PenStyle=PS_SOLID;
	m_DrawingMode=R2_COPYPEN;
	m_FirstPoint = 0;
	this->m_pointList.RemoveAll();
}
