#include "StdAfx.h"
#include ".\cmdimgdrawanyline.h"
#include "ObjCanvas.h"
#include "StdDefine.h"
extern void  aggDrawline_AA_8Bit (FCObjImage & img,
								  const POINT & pt1, const POINT & pt2,
								  int nWidth, int nGray,
								  BOOL bAddArrow, LINE_STYLE lineStyle) ;

//=================================================================================
// »­Ö±Ïß/draw line (32 bit)
void  CCmdImgDrawAnyLine::DrawAlphaMask (FCObjImage & imgAlpha) const
{
	if(m_pointList.GetCount()<=0)
		return;
	CPoint& p1=m_pointList.GetAt(0);
	for(int i=1;i<m_pointList.GetCount();++i){	
		CPoint& p2=m_pointList.GetAt(i);
		aggDrawline_AA_8Bit (imgAlpha,p1,p2, m_nPenWidth, m_nAlpha,FALSE, m_LineStyle) ;
		p1=p2;
	}
}
void  CCmdImgDrawAnyLine::OnPrepareBlockRect (const RECT & rcBlock)
{
	for(int i=0;i<m_pointList.GetCount();++i){	
		CPoint& p2=m_pointList.GetAt(i);
		p2.x=p2.x-rcBlock.left;
		p2.y=p2.y-rcBlock.top;
	}
}
void  CCmdImgDrawAnyLine::OnAfterGrowLayer (int nLeft, int nTop, int nRight, int nBottom)
{
	for(int i=0;i<m_pointList.GetCount();++i){	
		CPoint& p2=m_pointList.GetAt(i);
		p2.x=p2.x+nLeft;
		p2.y=p2.y+nTop;
	}
}
void  CCmdImgDrawAnyLine::QuerySaveRect (RECT * prcSave) const
{	
	int nminx,nminy,nmaxx,nmaxy;
	nminx=m_pointList.GetAt(0).x;
	nminy=m_pointList.GetAt(0).y;
	nmaxx=m_pointList.GetAt(0).x;
	nmaxy=m_pointList.GetAt(0).y;
	int size=m_pointList.GetCount();
	for(int i=0;i<size;++i){
		CPoint& p=m_pointList.GetAt(i);
		if(nminx>p.x)
			nminx=p.x;
		if(nminy>p.y)
			nminy=p.y;
		if(nmaxx<p.x)
			nmaxx=p.x;
		if(nmaxy<p.y)
			nmaxy=p.y;
	}	
	prcSave->top = nminy ;
	prcSave->bottom = nmaxy;
	prcSave->left = nminx;
	prcSave->right=nmaxx;
	::NormalizeRect (*prcSave) ;
	::InflateRect (prcSave, m_nPenWidth*2, m_nPenWidth*2) ;
	RECT		rcSel ;
	m_pCanvas->GetSelectLayerRect (*m_pLayer, &rcSel) ;
	::IntersectRect (prcSave, prcSave, &rcSel) ;
}

