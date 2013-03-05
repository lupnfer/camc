#pragma once
#include "objbase.h"
#include "ParentBrush.h"
#include "DHStroke.h"
#include "ObjImage.h"
#include "stdDefine.h"

class CParentPaper :
	public FCObject
{
public:
	CParentPaper(){m_prePosition.x=-99;m_prePosition.y=-99;}
	~CParentPaper(void);

	void InitBrush(CParentBrush* pBrush){
		m_pBrush = pBrush;
	}
	virtual void DrawSection(CDC *pDC,TPosition& p,COLORREF c);
	virtual void DrawSection(FCObjImage& img,TPosition& p,BYTE alpha);
	void GenTexture(LPSECTION lpSection,int r,COLORREF c);
	void SetPosition(int x,int y){
		m_prePosition.x=x;
		m_prePosition.y=y;
	}
private:
	COLORREF AlphaBlendPixel(COLORREF bgColor,COLORREF fgColor,int alphaPecent);
public:
	void DrawSection(FCObjImage& img,TPosition& p1,Vector2 v1,TPosition& p2,Vector2 v2,BYTE alpha);
protected:
	static BOOL IsOnSameSide(POINT p1,POINT P2,Point2 pA,Point2 pB);
protected:
	CParentBrush * m_pBrush;
	CPoint     m_prePosition;
};
