#pragma once
#include "cmdimgdrawlinebase.h"
#include "Curve.h"

class CCmdImgDrawAnyLine :
	public CCmdImgDrawlineBase
{
public:
	CCmdImgDrawAnyLine(CArray <CPoint,CPoint>& pointList,POINT offset,int nAlpha,RGBQUAD cr, int nPenWidth, LINE_STYLE LineStyle = LINE_STYLE_SOLID) : CCmdImgDrawlineBase(offset,nAlpha, cr, nPenWidth, LineStyle),m_pointList(pointList) 
	{
		for(int i=0;i<m_pointList.GetCount();++i){	
			CPoint& p2=m_pointList.GetAt(i);
			p2.x=p2.x-offset.x;
			p2.y=p2.y-offset.y;
		}
	}
public:
	virtual void  QuerySaveRect (RECT * prcSave) const ;
	virtual void  OnPrepareBlockRect (const RECT & rcBlock) ;
	virtual void  OnAfterGrowLayer (int nLeft, int nTop, int nRight, int nBottom) ;
	virtual void  DrawAlphaMask (FCObjImage & imgAlpha) const ;
protected:
	CArray <CPoint,CPoint>& m_pointList;	
};
