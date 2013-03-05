#pragma once
#include "unit.h"
#include "StdAfx.h"

class CCurve :public CUnit
{
private:
	CCurve(void);
public:
	~CCurve(void);
public:
	virtual void OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void Draw(CDC*pDC);
	virtual void Initial();
	virtual void DrawMask(CDC*pDC,CPoint first,CPoint second);
	static  CCurve* Instance();
private:
	CArray <CPoint,CPoint> m_pointList;
	CPoint m_FirstPoint;
	static CCurve* _instance;
};
