#include "StdAfx.h"
#include ".\paintbrush.h"
#include "objCanvas.h"
#include "DHBrush.h"
#include "DHPaper.h"
#include "DH2Brush.h"

#include "MainFrm.h"

CPaintBrush* CPaintBrush::_instance=NULL;
CPaintBrush::CPaintBrush(void)
{
	m_pStroke=NULL;
}

CPaintBrush::~CPaintBrush(void)
{
}


CPaintBrush* CPaintBrush::Instance()
{
	if(_instance==NULL){
		_instance=new CPaintBrush();
	}
	_instance->Initial();
	return _instance;
}
void CPaintBrush::OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point)
{
	if(m_p==point){
		return;
	}
	m_stroke.SetColor(m_PenColor);
	TPosition p;
	CPoint devp=point;
	pDC->LPtoDP(&devp);
	p.press=4;
	p.m_x=point.x;
	p.m_y=point.y;
	m_stroke.InsertStrokePiece(pDC,p);
	m_paper->DrawSection(pDC,p,m_PenColor);	
	m_paper->SetPosition(p.m_x,p.m_y);
	CUnit::OnLButtonDown(pDC,pView,point);
	m_p=point;

}
void CPaintBrush::OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point)
{
	m_rect=GetRect();
	pDC->LPtoDP(&m_rect);
	FCObjCanvas& objCanvas = pView->GetCanvas();
	RGBQUAD rgb;

	rgb.rgbBlue =GetBValue(m_PenColor);
	rgb.rgbGreen =GetGValue(m_PenColor);
	rgb.rgbRed = GetRValue(m_PenColor);
	rgb.rgbReserved = 0;	
	POINT offset=pView->GetCanvasOffset();
	//objCanvas.MapRealPoint (offset);
	DISCURVE& disCurve=m_stroke.GetDisCurve();
	DISCURVE::iterator it;
	
	for(it=disCurve.begin();it!=disCurve.end();++it)
	{
		POINT p;
		p.x=it->x;
		p.y=it->y;
		objCanvas.MapRealPoint(p);
		it->x=p.x;
		it->y=p.y;
	}
	objCanvas.MapRealPoint (offset);
	m_stroke.SetPenWidth(m_PenWidth);
	
	CUnit::OnLButtonUp(pDC,pView,point);
}
void CPaintBrush::OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point)
{
		
	if(pView->m_CurrentDrawStatus==Draw_Status)
	{
		if(m_p==point){
			return;
		}
		pDC->SetROP2(R2_COPYPEN);
		TPosition p;
		p.m_x=point.x;
		p.m_y=point.y;
		
		p.press=4;
		m_stroke.InsertStrokePiece(pDC,p);
		m_p=point;
	}
}
RECT CPaintBrush::GetRect()
{
	int nminx,nminy,nmaxx,nmaxy;
	DISCURVE::iterator it;
	DISCURVE& disCurve=m_stroke.GetDisCurve();

	if(disCurve.empty())
		return CUnit::GetRect();
	it=disCurve.begin();
	nminx=it->x;
	nminy=it->y;
	nmaxx=it->x;
	nmaxy=it->y;
	for(;it!=disCurve.end();++it){
		if(nminx>it->x)
			nminx=it->x;
		if(nminy>it->y)
			nminy=it->y;
		if(nmaxx<it->x)
			nmaxx=it->x;
		if(nmaxy<it->y)
			nmaxy=it->y;
	}
	RECT rcSave;
	rcSave.top = nminy ;
	rcSave.bottom = nmaxy;
	rcSave.left = nminx;
	rcSave.right=nmaxx;
	::NormalizeRect (rcSave) ;
	::InflateRect (&rcSave, m_PenWidth*2, m_PenWidth*2) ;
	return rcSave;
}
void CPaintBrush::Initial()
{
	CUnit::Initial();
	InitialBrushandPaper();
	m_stroke.Init(m_paper,m_PenColor);
	m_stroke.Clear();
	m_PenWidth=30;
}
//////////////////////////////////////////////////////////////////////////
void CPaintBrush::InitialBrushandPaper()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CCAMCView *pView = (CCAMCView*)pChild->GetActiveView();
	
}
void CPaintBrush::SetStroke(CBCStroke* pStroke)
{
	m_pStroke=pStroke;
}
void CPaintBrush::ShowSelectPoint(CDC *pDC,CCAMCView*pView,FCObjLayer* pLayer)
{
	if(m_pStroke==NULL)
		return;
	CBrush brush;
	brush.CreateSolidBrush(RGB(0,0,255));
	CPen m_pen;
	m_pen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	CPen *OldPen=pDC->SelectObject(&m_pen);
	int oldBkMode=pDC->SetBkMode(OPAQUE); 
	CBrush *OldBrush=pDC->SelectObject(&brush);
	int oldDrawingMode=pDC->SetROP2(R2_NOTXORPEN);
	RECT rt=m_pStroke->GetRect();
	BEZIERSTROKE& pList=m_pStroke->GetStroke();
	BEZIERSTROKE::iterator it;
	CRect rect;
	POINT pt;
	for(it=pList.begin();it!=pList.end();++it)
	{
		pt.x=it->bezCurve[0].x+rt.left;
		pt.y=it->bezCurve[0].y+rt.top;
		FCObjCanvas& objCanvas = pView->GetCanvas();
		objCanvas.LayerToCanvasPoint(*objCanvas.GetCurrentLayer(),pt);
		pView->CPToLP(pt);
		rect.SetRect(pt,pt);
		rect.InflateRect(3,3);
		pDC->Rectangle(rect);
	}
	if(it!=pList.begin()){
		it--;
		pt.x=it->bezCurve[3].x+rt.left;
		pt.y=it->bezCurve[3].y+rt.top;
		FCObjCanvas& objCanvas = pView->GetCanvas();
		objCanvas.LayerToCanvasPoint(*objCanvas.GetCurrentLayer(),pt);
		pView->CPToLP(pt);
		rect.SetRect(pt,pt);
		rect.InflateRect(3,3);
		pDC->Rectangle(rect);
	}

	pDC->SelectObject(OldPen);
	pDC->SetBkMode(oldBkMode); 
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(oldDrawingMode);
}
