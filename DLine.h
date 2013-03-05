#pragma once
#include "unit.h"
class CDLine :
	public CUnit
{
private:
	CDLine(void);
public:
	~CDLine(void);
public:
	virtual void OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void Draw(CDC*pDC);
	virtual void Initial();
	static CDLine* Instance();
private:
	CPoint m_FirstPoint;
	CPoint m_SecondPoint;
	static CDLine* _instance;
};
