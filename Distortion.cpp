#include "StdAfx.h"
#include ".\distortion.h"
#include "LayerCommand.h"
CDistortion* CDistortion::_instance = NULL;

CDistortion* CDistortion::Instance()
{
	if(_instance==NULL){
		_instance=new CDistortion();
	}
	_instance->Initial();
	return _instance;
}
CDistortion::CDistortion(void)
{
}

CDistortion::~CDistortion(void)
{
}
void CDistortion::OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point)
{
	
	if((pView->m_CurrentDrawStatus==Draw_Status))
	{
			SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR8));
			pDC->SetROP2(R2_NOTXORPEN);
			this->Draw(pDC);
			m_SecondPoint=point;
			this->Draw(pDC);
		}
}
void CDistortion::OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point)
{
	pView->m_CurrentDrawStatus=Draw_Status;

	switch(IsOnMarginPoint(point))
	{
	case 3:
		m_FirstPoint=m_SecondPoint;
		m_SecondPoint=point;
		break;
	case 4:
		m_FirstPoint=CPoint(m_FirstPoint.x,m_SecondPoint.y);
		m_SecondPoint=point;
		break;
	case 1:
		m_FirstPoint=m_FirstPoint;
		m_SecondPoint=point;
		break;
	case 2:
		m_FirstPoint=CPoint(m_SecondPoint.x,m_FirstPoint.y);
		m_SecondPoint=point;
		break;

	}
	ShowSelectPoint(pDC);
}
void CDistortion::OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point)
{
		CCAMCDoc* pDoc = pView->GetDocument();
		if (!pDoc)
			return ;
		int i = pDoc->GetLayerBar()->m_ListBox.GetCurSel();
		FCObjLayer *pLayer = pDoc->GetCanvas().GetLayer(i);
		int newwidth = m_SecondPoint.x - m_FirstPoint.x;
		int newheight = m_SecondPoint.y - m_FirstPoint.y;
		newwidth = fabs((double)newwidth);
		newheight = fabs((double)newheight);
		pDoc->GetCanvas().ExecuteEffect(new FCCmdLayerStretch(pLayer,newwidth,newheight));
		
		//CRect rc(m_FirstPoint,m_SecondPoint);
		pView->Invalidate();
		pView->m_CurrentDrawStatus=DEFAULT;
	
}
void CDistortion::Draw(CDC*pDC)
{

	CPen m_pen;
	m_pen.CreatePen(m_PenStyle,m_PenWidth,m_PenColor);
	CPen* pen=(CPen*)pDC->SelectObject(&m_pen);
	COLORREF OldColor=pDC->SetBkColor(m_BackgroundColor);
	int OldBkMode=pDC->SetBkMode(m_BkMode);
	pDC->Rectangle(CRect(m_FirstPoint,m_SecondPoint)); 
	pDC->SelectObject(pen);
	pDC->SetBkMode(m_BkMode);
	pDC->SetBkColor(OldColor);

}
int CDistortion::IsOnMarginPoint(CPoint point)
{
	CRect rect(point,point);
	rect.InflateRect(CSize(3,3));
	if(rect.PtInRect(m_FirstPoint))
	{   
		m_FoundPoint=m_FirstPoint;
		return 1;
	}
	if(rect.PtInRect(CPoint(m_SecondPoint.x,m_FirstPoint.y)))
	{   m_FoundPoint=(CPoint(m_SecondPoint.x,m_FirstPoint.y));
	return 2;
	}
	if(rect.PtInRect(m_SecondPoint))
	{	 
		m_FoundPoint=m_SecondPoint;
		return 3;
	}
	if(rect.PtInRect(CPoint(m_FirstPoint.x,m_SecondPoint.y)))
	{   m_FoundPoint=CPoint(m_FirstPoint.x,m_SecondPoint.y);
	return 4;
	}
	
	return 0;

}

void CDistortion::ShowSelectPoint(CDC *pDC)
{
	CBrush brush;
	brush.CreateSolidBrush(RGB(0,255,0));
	CPen m_pen;
	m_pen.CreatePen(PS_SOLID,1,RGB(0,255,0));
	CPen *OldPen=pDC->SelectObject(&m_pen);
	int oldBkMode=pDC->SetBkMode(OPAQUE); 
	CBrush *OldBrush=pDC->SelectObject(&brush);
	int oldDrawingMode=pDC->SetROP2(R2_NOTXORPEN);
	CRect rect;

	rect.SetRect(this->m_FirstPoint,this->m_FirstPoint);
	rect.InflateRect(3,3);
	pDC->Rectangle(rect);

	rect.SetRect(this->m_SecondPoint,this->m_SecondPoint);
	rect.InflateRect(3,3);
	pDC->Rectangle(rect);

	rect.SetRect(this->m_FirstPoint.x,this->m_SecondPoint.y,this->m_FirstPoint.x,this->m_SecondPoint.y);
	rect.InflateRect(3,3);
	pDC->Rectangle(rect);

	rect.SetRect(this->m_SecondPoint.x,this->m_FirstPoint.y,this->m_SecondPoint.x,this->m_FirstPoint.y);
	rect.InflateRect(3,3);
	pDC->Rectangle(rect);

	
	pDC->SelectObject(OldPen);
	pDC->SetBkMode(oldBkMode); 
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(oldDrawingMode);

}
void CDistortion::Initial()
{
	m_BkMode=TRANSPARENT;
	m_PenColor=RGB(0,0,0);
	m_BackgroundColor=RGB(255,255,255);
	m_PenWidth=1;
	m_PenStyle=PS_SOLID;
	m_DrawingMode=R2_COPYPEN;


	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CCAMCDoc* pDoc =(CCAMCDoc*)(pChild->GetActiveDocument());
	CCAMCView* pView = (CCAMCView*)(pChild->GetActiveView());
	POINT ptoffset = pView->GetCanvasOffset();

	FCObjCanvas& objCanvas = pDoc->GetCanvas();
	if (!pDoc)
		return ;
	int i = pDoc->GetLayerBar()->m_ListBox.GetCurSel();
	FCObjLayer *pLayer = objCanvas.GetLayer(i);
	RECT rc;
	pLayer->GetRectInCanvas(&rc);
	objCanvas.MapScaledRect(rc);
	m_FirstPoint.x = rc.left;
	m_FirstPoint.y = rc.top;
	m_SecondPoint.x = rc.right;
	m_SecondPoint.y = rc.bottom;

	m_FirstPoint += ptoffset;
	m_SecondPoint += ptoffset;
}