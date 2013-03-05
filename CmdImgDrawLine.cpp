#include "StdAfx.h"
#include ".\cmdimgdrawline.h"
#include "ObjCanvas.h"
extern void  aggDrawline_AA_8Bit (FCObjImage & img,
								  const POINT & pt1, const POINT & pt2,
								  int nWidth, int nGray,
								  BOOL bAddArrow, LINE_STYLE lineStyle) ;

//=================================================================================
// »­Ö±Ïß/draw line (32 bit)
void  CCmdImgDrawLine::DrawAlphaMask (FCObjImage & imgAlpha) const
{
	aggDrawline_AA_8Bit (imgAlpha, m_pt1, m_pt2, m_nPenWidth, m_nAlpha, m_bArrow, m_LineStyle) ;
}
void  CCmdImgDrawLine::OnPrepareBlockRect (const RECT & rcBlock)
{
	m_pt1.x -= rcBlock.left ; m_pt2.x -= rcBlock.left ;
	m_pt1.y -= rcBlock.top ; m_pt2.y -= rcBlock.top ;
}
void  CCmdImgDrawLine::OnAfterGrowLayer (int nLeft, int nTop, int nRight, int nBottom)
{
	m_pt1.x += nLeft ; m_pt2.x += nLeft ;
	m_pt1.y += nTop ; m_pt2.y += nTop ;
}
void  CCmdImgDrawLine::QuerySaveRect (RECT * prcSave) const
{
	prcSave->left = m_pt1.x ; prcSave->top = m_pt1.y ;
	prcSave->right = m_pt2.x ; prcSave->bottom = m_pt2.y ;
	::NormalizeRect (*prcSave) ;
	::InflateRect (prcSave, m_nPenWidth*2, m_nPenWidth*2) ;
	RECT		rcSel ;
	m_pCanvas->GetSelectLayerRect (*m_pLayer, &rcSel) ;
	::IntersectRect (prcSave, prcSave, &rcSel) ;

}
	
