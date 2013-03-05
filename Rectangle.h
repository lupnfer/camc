#pragma once
#include "unit.h"

class CRectangle :
	public CUnit
{
public:
	CRectangle(void);
	~CRectangle(void);
public:
	virtual void OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void Draw(CDC*pDC);
	virtual void Initial();
	static  CRectangle* Instance();
public:
	CPoint m_FirstPoint;
	CPoint m_SecondPoint;
	static CRectangle* _instance;

};
