#include "StdAfx.h"
#include ".\rotate.h"
#include "LayerCommand.h"
CRotate* CRotate::_instance = NULL;

CRotate* CRotate::Instance()
{
	if(_instance==NULL){
		_instance=new CRotate();
	}
	_instance->Initial();
	return _instance;
}
CRotate::CRotate(void)
{

}

CRotate::~CRotate(void)
{
}
void CRotate::OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point)
{
	if((pView->m_CurrentDrawStatus==Draw_Status))
	{
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR7));
		pDC->SetROP2(R2_NOTXORPEN);
		
		InvalidateSelf(pView);	
		this->Draw(pDC);
		double dangle = FPointAngleClockwise(m_FirstPoint,point,m_ptCenter);
		m_angle = RadianToAngle(dangle);
		
		m_LTPoint = FClockwisePoint(m_LTPoint_Orign,m_ptCenter,dangle);
		m_RTPoint = FClockwisePoint(m_RTPoint_Orign,m_ptCenter,dangle);
		m_LBPoint = FClockwisePoint(m_LBPoint_Orign,m_ptCenter,dangle);
		m_RBPoint = FClockwisePoint(m_RBPoint_Orign,m_ptCenter,dangle);
		/*if(m_nIndex==1)
			m_FirstPoint=m_LTPoint;
		else if(m_nIndex==2)
			m_FirstPoint=m_RTPoint;
		else if(m_nIndex==3)
			m_FirstPoint=m_LBPoint;
		else m_FirstPoint=m_RBPoint;*/

		this->Draw(pDC);

	}
}
void CRotate::OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point)
{
/*	FCObjCanvas& objCanvas = pView->GetCanvas();
	CCAMCDoc* pDoc = pView->GetDocument();
	if (!pDoc)
		return ;
	int i = pDoc->GetLayerBar()->m_ListBox.GetCurSel();
	FCObjLayer *pLayer = objCanvas.GetLayer(i);
	RECT rc;
	pLayer->GetRectInCanvas(&rc);*/
	ShowSelectPoint(pDC);
	//this->Initial();
	m_nIndex = IsOnMarginPoint(point);
	switch(m_nIndex) 
	{
		case 1:
			pView->m_CurrentDrawStatus=Draw_Status;
			m_FirstPoint = m_LTPoint; 
			break;
		case 2:
			pView->m_CurrentDrawStatus=Draw_Status;
			m_FirstPoint = m_RTPoint; 
			break;
		case 3:
			pView->m_CurrentDrawStatus=Draw_Status;
			m_FirstPoint = m_LBPoint; 
			break;
		case 4:
			pView->m_CurrentDrawStatus=Draw_Status;
			m_FirstPoint = m_RBPoint; 
			break;
		default:
			break;
	}
}
void CRotate::OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point)
{
	
		CCAMCDoc* pDoc = pView->GetDocument();
		if (!pDoc)
			return ;
		int i = pDoc->GetLayerBar()->m_ListBox.GetCurSel();
		FCObjLayer *pLayer = pDoc->GetCanvas().GetLayer(i);
		pDoc->GetCanvas().ExecuteEffect(new FCCmdLayerRotate(pLayer,m_angle));
		ShowSelectPoint(pDC);
		pView->m_CurrentDrawStatus=DEFAULT;
		pView->Invalidate();
}
int CRotate::IsOnMarginPoint(CPoint point)
{
	CRect rect1;	
	rect1.SetRect(m_LTPoint,m_LTPoint);
	rect1.InflateRect(3,3);

	CRect rect2;
	rect2.SetRect(m_RTPoint,m_RTPoint);
	rect2.InflateRect(3,3);
	
	CRect rect3;	
	rect3.SetRect(m_LBPoint,m_LBPoint);
	rect3.InflateRect(3,3);
	
	CRect rect4;	
	rect4.SetRect(m_RBPoint,m_RBPoint);
	rect4.InflateRect(3,3);
	if (rect1.PtInRect(point))
		{
			return 1;
		}
	if (rect2.PtInRect(point))
		{
			return 2;
		}
	if (rect3.PtInRect(point))
		{
			return 3;
		}
	if (rect4.PtInRect(point))
		{
			return 4;
		}
	else 
		return 0;
}
void CRotate::ShowSelectPoint(CDC *pDC)
{

	CBrush brush;
	brush.CreateSolidBrush(RGB(0,255,0));
	CPen m_pen;
	m_pen.CreatePen(PS_SOLID,1,RGB(0,255,0));
	CPen *OldPen=pDC->SelectObject(&m_pen);
	int oldBkMode=pDC->SetBkMode(OPAQUE); 
	CBrush *OldBrush=pDC->SelectObject(&brush);
	int oldDrawingMode=pDC->SetROP2(R2_COPYPEN);

	CRect rect;

	rect.SetRect(m_LTPoint,m_LTPoint);
	rect.InflateRect(3,3);
	pDC->Rectangle(rect);
	rect.SetRect(m_RTPoint,m_RTPoint);
	rect.InflateRect(3,3);
	pDC->Rectangle(rect);
	rect.SetRect(m_RBPoint,m_RBPoint);
	rect.InflateRect(3,3);
	pDC->Rectangle(rect);
	rect.SetRect(m_LBPoint,m_LBPoint);
	rect.InflateRect(3,3);
	pDC->Rectangle(rect);
	rect.SetRect(m_ptCenter,m_ptCenter);
	rect.InflateRect(3,3);
	pDC->Rectangle(rect);

	pDC->MoveTo(m_LTPoint);
	pDC->LineTo(m_RTPoint);
	pDC->MoveTo(m_RTPoint);
	pDC->LineTo(m_RBPoint);
	pDC->MoveTo(m_RBPoint);
	pDC->LineTo(m_LBPoint);
	pDC->MoveTo(m_LBPoint);
	pDC->LineTo(m_LTPoint);

	pDC->SelectObject(OldPen);
	pDC->SetBkMode(oldBkMode); 
	pDC->SetROP2(oldDrawingMode);
}

void CRotate::Draw(CDC*pDC)
{
	CPen m_pen;
	m_pen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	CPen* pen=(CPen*)pDC->SelectObject(&m_pen);
	COLORREF OldColor=pDC->SetBkColor(m_BackgroundColor);
	int OldBkMode=pDC->SetBkMode(m_BkMode);

	pDC->MoveTo(m_LTPoint);
	pDC->LineTo(m_RTPoint);
	pDC->MoveTo(m_RTPoint);
	pDC->LineTo(m_RBPoint);
	pDC->MoveTo(m_RBPoint);
	pDC->LineTo(m_LBPoint);
	pDC->MoveTo(m_LBPoint);
	pDC->LineTo(m_LTPoint);

	pDC->SelectObject(pen);
	pDC->SetBkMode(m_BkMode);
	pDC->SetBkColor(OldColor);

}
void CRotate::Initial()
{
	m_BkMode=TRANSPARENT;
	m_PenColor=RGB(0,0,0);
	m_BackgroundColor=RGB(255,255,255);
	m_PenWidth=1;
	m_PenStyle=PS_SOLID;
	m_DrawingMode=R2_COPYPEN;
	
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CCAMCView* pView = (CCAMCView*)(pChild->GetActiveView());
	CCAMCDoc* pDoc =(CCAMCDoc*)(pChild->GetActiveDocument());


	FCObjCanvas& objCanvas = pDoc->GetCanvas();
	if (!pDoc)
		return ;
	int i = pDoc->GetLayerBar()->m_ListBox.GetCurSel();
	FCObjLayer *pLayer = objCanvas.GetLayer(i);
	RECT rc;
	pLayer->GetRectInCanvas(&rc);
	objCanvas.MapScaledRect(rc);
	POINT ptoffset = pView->GetCanvasOffset();
	m_LTPoint.x = rc.left;
	m_LTPoint.y = rc.top;
	m_LTPoint+=ptoffset;
	m_RTPoint.x = rc.right;
	m_RTPoint.y = rc.top;
	m_RTPoint+=ptoffset;
	m_LBPoint.x = rc.left;
	m_LBPoint.y = rc.bottom;
	m_LBPoint+=ptoffset;
	m_RBPoint.x = rc.right;
	m_RBPoint.y = rc.bottom;
	m_RBPoint+=ptoffset;
	m_ptCenter.x = (m_LTPoint.x+m_RBPoint.x)/2;
	m_ptCenter.y = (m_LTPoint.y+m_RBPoint.y)/2;	
	m_angle=0;

	m_LTPoint_Orign=m_LTPoint;
	m_RTPoint_Orign=m_RTPoint;
	m_LBPoint_Orign=m_LBPoint;
	m_RBPoint_Orign=m_RBPoint;
}
void CRotate::InvalidateSelf(CCAMCView* pView)
{
	CRect rect;
	rect.SetRect(m_LTPoint,m_LTPoint);
	rect.InflateRect(4,4);
	pView->InvalidateRect(rect);
	rect.SetRect(m_RTPoint,m_RTPoint);
	rect.InflateRect(4,4);
	pView->InvalidateRect(rect);
	rect.SetRect(m_RBPoint,m_RBPoint);
	rect.InflateRect(4,4);
	pView->InvalidateRect(rect);
	rect.SetRect(m_LBPoint,m_LBPoint);
	rect.InflateRect(4,4);
	pView->InvalidateRect(rect);
	rect.SetRect(m_ptCenter,m_ptCenter);
	rect.InflateRect(4,4);
	pView->InvalidateRect(rect);
	m_nIndex=0;
}