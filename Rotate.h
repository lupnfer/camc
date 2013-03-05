#pragma once
#include "unit.h"

class CRotate :
	public CUnit
{
public:
	CRotate(void);
	~CRotate(void);
	virtual void OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void ShowSelectPoint(CDC *pDC);
	virtual void Draw(CDC*pDC);
	virtual int IsOnMarginPoint(CPoint point);
	void InvalidateSelf(CCAMCView* pView);
	
	void Initial();
	static CRotate* Instance();
public:
	CPoint m_LTPoint;          //×óÉÏ½Çµã
	CPoint m_RTPoint;
	CPoint m_LBPoint;
	CPoint m_RBPoint;
	CPoint m_ptCenter;
	CPoint m_FirstPoint;
	CPoint m_SecondPoint;
	int    m_nIndex;
	CPoint m_LTPoint_Orign;
	CPoint m_RTPoint_Orign;
	CPoint m_LBPoint_Orign;
	CPoint m_RBPoint_Orign;


private:
	int  m_angle;
	static CRotate* _instance;
};
