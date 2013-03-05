#pragma once
#include "cmdimgdrawlinebase.h"
#include "CAMCView.h"
class CCmdImgDrawLine :public CCmdImgDrawlineBase
{
public :
	CCmdImgDrawLine (POINT offset,int nAlpha, POINT pt1, POINT pt2, 
					RGBQUAD cr, int nPenWidth, LINE_STYLE LineStyle = LINE_STYLE_SOLID, BOOL bArrow = FALSE) : 
					CCmdImgDrawlineBase(offset,nAlpha, cr, nPenWidth, LineStyle),m_bArrow(bArrow),m_pt1(pt1),m_pt2(pt2) 
	{
		
		m_pt1.x-=offset.x;
		m_pt1.y-=offset.y;
		m_pt2.x-=offset.x;
		m_pt2.y-=offset.y;

	}
protected :
		virtual void  QuerySaveRect (RECT * prcSave) const ;
		virtual void  OnPrepareBlockRect (const RECT & rcBlock) ;
		virtual void  OnAfterGrowLayer (int nLeft, int nTop, int nRight, int nBottom) ;
		virtual void  DrawAlphaMask (FCObjImage & imgAlpha) const ;
public  :
		POINT       m_pt1, m_pt2 ; // 坐标为m_pLayer上的坐标
		BOOL        m_bArrow ; // 是否画箭头
};
