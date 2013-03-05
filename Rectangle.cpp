#include "StdAfx.h"
#include ".\rectangle.h"
#include "CAMCDoc.h"
#include "CmdSelectionSet.h"

CRectangle* CRectangle::_instance=NULL;
CRectangle::CRectangle(void)
{
	//Initial();
}

CRectangle::~CRectangle(void)
{
}
CRectangle* CRectangle::Instance()
{
	if(_instance==NULL){
		_instance=new CRectangle();
	}
	_instance->Initial();
	return _instance;
}
void CRectangle::OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point)
{
	m_FirstPoint=point;
	m_SecondPoint=point;
	pView->m_CurrentDrawStatus=Draw_Status;
}
void CRectangle::OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point)
{
	if(pView->m_CurrentDrawStatus==Draw_Status){

		pDC->SetROP2(R2_NOTXORPEN);
		this->Draw(pDC);
		m_SecondPoint=point;
		this->Draw(pDC);
	}

}
void CRectangle::OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point)
{
	m_SecondPoint=point;
	FCObjCanvas& objCanvas = pView->GetCanvas();
	FCObjLayer *pLayer = objCanvas.GetCurrentLayer();
	FCObjSelect		fobjSelect;	
	
	
	objCanvas.MapRealPoint (m_FirstPoint);
	objCanvas.MapRealPoint (m_SecondPoint);

	if (objCanvas.HasSelected()) {
		objCanvas.__ClearSelection();
	}


	POINT ptoffset = pView->GetCanvasOffset();
	objCanvas.MapRealPoint(ptoffset);

	CRect rcNew = CRect(0,0,0,0);	
	rcNew.top = m_FirstPoint.y-ptoffset.y;
	rcNew.bottom = m_SecondPoint.y-ptoffset.y;
	rcNew.left = m_FirstPoint.x-ptoffset.x;
	rcNew.right = m_SecondPoint.x-ptoffset.x;
	


	fobjSelect.SetRect(rcNew,FCObjSelect::RGN_CREATE);
	objCanvas.ExecuteEffect(new CCmdSelectionSet(fobjSelect));
	pView->m_CurrentDrawStatus = DEFAULT;
	//pView->Invalidate();
}
void CRectangle::Draw(CDC*pDC)
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
void CRectangle::Initial()
{
	m_BkMode=TRANSPARENT;
	m_PenColor=RGB(0,0,0);
	m_BackgroundColor=RGB(255,255,255);
	m_PenWidth=1;
	m_PenStyle=PS_DOT;
	m_DrawingMode=R2_COPYPEN;
	m_FirstPoint = 0;
	m_SecondPoint = 0 ;
}