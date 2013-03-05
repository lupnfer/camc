#pragma once
#include "cmdimgdrawlinebase.h"
#include "BsplineCurve.h"

class CCmdImgDrawBSpline :
	public CCmdImgDrawlineBase
{
public:
	CCmdImgDrawBSpline(double* Qx,double* Qy,DWORD dwCount,
						POINT offset,int nAlpha,RGBQUAD cr, int nPenWidth, 
						LINE_STYLE LineStyle = LINE_STYLE_SOLID)
						:CCmdImgDrawlineBase(offset,nAlpha, cr, nPenWidth, LineStyle),m_dwCount(dwCount)
	{
		for(int i=1;i<=m_dwCount;++i){
			m_Qx[i]=Qx[i]-m_offset.x;
			m_Qy[i]=Qy[i]-m_offset.y;
		}
		CBsplineCurve::ComputeControlPoints(dwCount,m_Qx,m_Px);
		CBsplineCurve::ComputeControlPoints(dwCount,m_Qy,m_Py);
	}

	virtual ~CCmdImgDrawBSpline(void){}
public:
	virtual void  QuerySaveRect (RECT * prcSave) const ;
	virtual void  OnPrepareBlockRect (const RECT & rcBlock) ;
	virtual void  OnAfterGrowLayer (int nLeft, int nTop, int nRight, int nBottom) ;
	virtual void  DrawAlphaMask (FCObjImage & imgAlpha) const ;
private:
	double m_Qx[N+1], m_Qy[N+1];
	double m_Px[N+2], m_Py[N+2];
	DWORD  m_dwCount;		// number of passing points of each curve
	int    m_bGWidth;
};
