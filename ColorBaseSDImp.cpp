#include "StdAfx.h"
#include ".\colorbasesdimp.h"
#include "CAMCView.h"
CColorBaseSDImp::CColorBaseSDImp(void)
{
}

CColorBaseSDImp::~CColorBaseSDImp(void)
{
}

void CColorBaseSDImp::OnLButtonDown(CDC *pDC, CCAMCView *pView, CPoint point)
{
	if(pView->m_CurrentTool==COLORBASESDIMP){
		pView->m_CurrentDrawStatus=Draw_Status;
		m_FirstPoint=point;
	}
	m_PenColor=RGB(0,0,255);
}

void CColorBaseSDImp::GetSelectedPixels(COLORREF& c)
{
	LONG sumRColor=0,sumGColor=0,sumBColor=0;
	BYTE Rvalue,Gvalue,Bvalue;
	Rvalue = Gvalue = Bvalue =0;
	CPoint currentpoint;
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CCAMCDoc* pDoc =(CCAMCDoc*)(pChild->GetActiveDocument());
	int index = pDoc->GetLayerBar()->m_ListBox.GetCurSel();
	FCObjLayer * layer = pDoc->GetCanvas()->GetLayer(index);

	for(int i = 0 ; i < m_pointList.GetCount(); i++)
	{
		currentpoint = m_pointList.GetAt(i);
		layer->GetPixel(currentpoint.x,currentpoint.y,Rvalue,Gvalue,Bvalue);
		sumRColor += Rvalue;
		sumGColor += Gvalue;
		sumBColor += Bvalue;
	}
	Rvalue = sumRColor/m_pointList.GetCount();
	Gvalue = sumGColor/m_pointList.GetCount();
	Bvalue = sumBColor/m_pointList.GetCount();
	c = RGB(Rvalue,Gvalue,Bvalue);
}

COLORREF CColorBaseSDImp::GetMeanColor()
{
	return m_meancolor;
}
void CColorBaseSDImp::SetMeanColor()
{
	COLORREF c;
	GetSelectedPixels(c);
	m_meancolor = c;
}
