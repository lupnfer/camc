#pragma once
#include "unit.h"

class CRubber :
	public CUnit
{
public:
	CRubber(void);
	~CRubber(void);
public:
	virtual void OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void Draw(CDC*pDC);
	virtual void Initial();
	virtual void DrawMask(CDC*pDC,CPoint first,CPoint second);

public:
	CArray <CPoint,CPoint> m_pointList;
	CPoint m_FirstPoint;
};
