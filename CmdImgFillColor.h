#pragma once
#include "cmdimgeffect.h"

class CCmdImgFillColor :
	public CCmdImgEffect
{
public:
	CCmdImgFillColor(void);
	~CCmdImgFillColor(void);
public :
	CCmdImgFillColor (CPoint point,COLORREF color) : m_point(point),m_color(color) {}
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) ;
protected :
	COLORREF m_color;
	CPoint   m_point;

};
