#include "StdAfx.h"
#include "ObjCanvas.h"
#include "BsplineCurve.h"
#include ".\cmdimgdrawbspline.h"

extern void  aggDrawline_AA_8Bit (FCObjImage & img,
								  const POINT & pt1, const POINT & pt2,
								  int nWidth, int nGray,
								  BOOL bAddArrow, LINE_STYLE lineStyle) ;
//=================================================================================

void  CCmdImgDrawBSpline::DrawAlphaMask (FCObjImage & imgAlpha) const
{
	if(m_dwCount == 2)
	{
		aggDrawline_AA_8Bit(imgAlpha,CPoint((int)m_Qx[1], (int)m_Qy[1]),
							CPoint((int)m_Qx[2], (int)m_Qy[2]),m_nPenWidth,RGB(m_cr.rgbRed,m_cr.rgbGreen,m_cr.rgbBlue),false,m_LineStyle);	
	}
	if(m_dwCount>= 3){
		for(int i = 0; i <= m_dwCount-2; i++)
			CBsplineCurve::Draw1BSpline(imgAlpha, m_Px+i, m_Py+i,m_nPenWidth,RGB(m_cr.rgbRed,m_cr.rgbGreen,m_cr.rgbBlue),m_LineStyle);
	}	
}
void  CCmdImgDrawBSpline::OnPrepareBlockRect (const RECT & rcBlock)
{
	m_offset.x+= rcBlock.left;
	m_offset.y+= rcBlock.top ;
}
void  CCmdImgDrawBSpline::OnAfterGrowLayer (int nLeft, int nTop, int nRight, int nBottom)
{
	m_offset.x+=nLeft;
	m_offset.y+=nTop;
}
void  CCmdImgDrawBSpline::QuerySaveRect (RECT * prcSave) const
{	
	int nminx,nminy,nmaxx,nmaxy;	
	nminx=m_Px[0];
	nminy=m_Py[0];
	nmaxx=m_Px[0];
	nmaxy=m_Py[0];
	for(int i=1;i<=m_dwCount+1;++i){
		if(nminx>m_Px[i])
			nminx=m_Px[i];
		if(nminy>m_Py[i])
			nminy=m_Py[i];
		if(nmaxx<m_Px[i])
			nmaxx=m_Px[i];
		if(nmaxy<m_Py[i])
			nmaxy=m_Py[i];
	}	
	prcSave->top = nminy;
	prcSave->bottom = nmaxy;
	prcSave->left = nminx;
	prcSave->right=nmaxx;
	::NormalizeRect (*prcSave) ;
	::InflateRect (prcSave, m_nPenWidth*2, m_nPenWidth*2) ;
	RECT		rcSel ;
	m_pCanvas->GetSelectLayerRect (*m_pLayer, &rcSel) ;
	::IntersectRect (prcSave, prcSave, &rcSel) ;	
}
