#pragma once
#include "unit.h"

class CDistortion:public CUnit
{
public:
	CDistortion(void);
	~CDistortion(void);
	virtual void OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void Draw(CDC*pDC);
	virtual int IsOnMarginPoint(CPoint point);
	virtual void ShowSelectPoint(CDC *pDC);
	virtual void Initial();
	static CDistortion* Instance();

public:
	CPoint m_FirstPoint;
	CPoint m_SecondPoint;
	CPoint m_FoundPoint;
private:
	static CDistortion* _instance;
};
