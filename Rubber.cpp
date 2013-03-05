#include "StdAfx.h"
#include ".\rubber.h"
#include "CmdImgDrawline.h"
#include "CAMCDoc.h"
CRubber::CRubber(void)
{
	Initial();
}

CRubber::~CRubber(void)
{
}
void CRubber::OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point)
{
	m_FirstPoint=point;
	pView->m_CurrentDrawStatus=Draw_Status;

}
void CRubber::OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point)
{
	pView->m_CurrentDrawStatus=DEFAULT;
}
void CRubber::DrawMask(CDC*pDC,CPoint first,CPoint second)
{
	CPen m_pen;
	m_pen.CreatePen(m_PenStyle,m_PenWidth,m_PenColor);
	CPen* pen=(CPen*)pDC->SelectObject(&m_pen);
	pDC->MoveTo(first);
	pDC->LineTo(second);
	pDC->SelectObject(pen);
}

void CRubber::OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point)
{

	if(pView->m_CurrentDrawStatus==Draw_Status)
	{
		//pDC->SetROP2(R2_WHITE);
		pDC->SetROP2(R2_COPYPEN);
		m_pointList.Add(point);
		m_DrawingMode=pDC->GetROP2();
		DrawMask(pDC,m_FirstPoint,point);
		//////////////////////////////////////////////////////////////////////////
		CCAMCDoc* pDoc = pView->GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc)
			return;	
		FCObjCanvas& objCanvas = pDoc->GetCanvas();
		RGBQUAD rgb;
			rgb.rgbBlue =255;
			rgb.rgbGreen =255;
			rgb.rgbRed = 255;
			rgb.rgbReserved = 0;
		CRect rect;
		pView->GetClientRect(&rect);
		SIZE rcsize;
		rcsize.cx = rect.right - rect.left;
		rcsize.cy = rect.bottom - rect.top;
		POINT ptoffset = objCanvas.GetViewOffset(rcsize);

		POINT pt1,pt2;
		pt1.x = m_FirstPoint.x - ptoffset.x;
		pt1.y = m_FirstPoint.y - ptoffset.y ;
		pt2.x = point.x - ptoffset.x;
		pt2.y = point.y - ptoffset.y;	
		objCanvas.ExecuteEffect(new CCmdImgDrawLine(255,pt1,pt2,rgb,m_PenWidth));

		//////////////////////////////////////////////////////////////////////////

		m_FirstPoint=point;
	}
}
void CRubber::Draw(CDC*pDC)
{
	CPen m_pen;	
	if(m_pointList.IsEmpty())
		return;
	m_pen.CreatePen(m_PenStyle,m_PenWidth,m_PenColor);
	CPen* pen=(CPen*)pDC->SelectObject(&m_pen);
	int oldDrawingMode=pDC->SetROP2(m_DrawingMode);
	m_FirstPoint=m_pointList.GetAt(0);
	pDC->MoveTo(m_FirstPoint);
	for(int i=0;i<m_pointList.GetSize();i++)
	{
		pDC->LineTo(m_pointList.GetAt(i));
		pDC->LineTo(m_pointList.GetAt(i));
	}
	pDC->LineTo(m_pointList.GetAt(i-1));
	pDC->SelectObject(pen);
	pDC->SetROP2(oldDrawingMode);  

}
void CRubber::Initial()
{
	m_BkMode=OPAQUE;
	m_PenColor=RGB(0,0,0);
	m_BackgroundColor=RGB(255,255,255);
	m_PenWidth=15;
	m_PenStyle=PS_SOLID;
	m_DrawingMode=R2_COPYPEN;
	m_FirstPoint = 0;
}
