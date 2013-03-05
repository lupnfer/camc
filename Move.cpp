#include "StdAfx.h"
#include ".\move.h"
#include "LayerCommand.h"
CMove* CMove::_instance = NULL;
CMove* CMove::Instance()
{
	if(_instance==NULL){
		_instance=new CMove();
	}
	_instance->Initial();
	return _instance;
}

CMove::CMove(void)
{

}

CMove::~CMove(void)
{

}
void CMove::OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point)
{
	if((pView->m_CurrentDrawStatus==Draw_Status))
	{
		//FCObjCanvas& objCanvas = pView->GetCanvas();
		//objCanvas.MapScaledPoint(point);
		pDC->SetROP2(R2_NOTXORPEN);
		this->Draw(pDC);
		m_SecondPoint += (point-m_FindPoint);
		m_FirstPoint += (point-m_FindPoint);
		this->Draw(pDC);
		m_FindPoint = point;
		
	}
}

void CMove::OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point)
{
		FCObjCanvas& objCanvas = pView->GetCanvas();
		CCAMCDoc *pDoc = pView->GetDocument();
		
		if (!pDoc)
			return ;
		int i = pDoc->GetLayerBar()->m_ListBox.GetCurSel();
		FCObjLayer *pLayer = objCanvas.GetLayer(i);
		m_offset.x=point.x-m_offset.x;
		m_offset.y=point.y-m_offset.y;
		objCanvas.MapRealPoint(m_offset);
		m_ptOld.x+=m_offset.x;
		m_ptOld.y+=m_offset.y;
		objCanvas.ExecuteEffect(new FCCmdLayerMove(pLayer,m_ptOld));
		pView->Invalidate();
		pView->m_CurrentDrawStatus=DEFAULT;
}
void CMove::OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point)
{
	FCObjCanvas& objCanvas = pView->GetCanvas();

	pView->m_CurrentDrawStatus=Draw_Status;
	m_FindPoint = point;
	m_offset.x=point.x;
	m_offset.y=point.y;
	//objCanvas.MapScaledPoint(m_FindPoint);
}

void CMove::ShowSelectPoint(CDC *pDC)
{
	CBrush brush;
	brush.CreateSolidBrush(RGB(0,0,255));
	CPen m_pen;
	m_pen.CreatePen(PS_SOLID,1,RGB(0,0,255));
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

	pDC->SelectObject(OldBrush);
	brush.DeleteObject();
	brush.CreateSolidBrush(RGB(255,0,0));
	pDC->SelectObject(&brush);
	rect.SetRect((this->m_SecondPoint.x+this->m_FirstPoint.x)/2,(this->m_SecondPoint.y+this->m_FirstPoint.y)/2,(this->m_SecondPoint.x+this->m_FirstPoint.x)/2,(this->m_SecondPoint.y+this->m_FirstPoint.y)/2);
	rect.InflateRect(3,3);
	pDC->Rectangle(rect);
	pDC->SelectObject(OldPen);
	pDC->SetBkMode(oldBkMode); 
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(oldDrawingMode);
}

void CMove::Draw(CDC*pDC)
{
	CPen m_pen;
	m_pen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	CPen* pen=(CPen*)pDC->SelectObject(&m_pen);
	COLORREF OldColor=pDC->SetBkColor(m_BackgroundColor);
	int OldBkMode=pDC->SetBkMode(m_BkMode);

	pDC->Rectangle(m_FirstPoint.x,m_FirstPoint.y,m_SecondPoint.x,m_SecondPoint.y);

	pDC->SelectObject(pen);
	pDC->SetBkMode(m_BkMode);
	pDC->SetBkColor(OldColor);

}

void CMove::Initial()
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
	m_ptOld=pLayer->GetGraphObjPos();
	m_offset.x=0;
	m_offset.y=0;
}