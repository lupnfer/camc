#pragma once
#include "unit.h"

class CMove :
	public CUnit
{
public:
	CMove(void);
	~CMove(void);
	virtual void OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void ShowSelectPoint(CDC *pDC);
	virtual void Draw(CDC*pDC);
	virtual void Initial();
	static CMove* Instance();
private:
	CPoint m_FindPoint;
	CPoint m_FirstPoint;
	CPoint m_SecondPoint;
	static CMove* _instance;
	POINT  m_ptOld;
	POINT  m_offset;
};
