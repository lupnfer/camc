
#pragma once
#include "curve.h"
class CCAMCView;
class CColorBaseSDImp :public CCurve
{

public:
	CColorBaseSDImp();
	~CColorBaseSDImp();
	virtual void OnLButtonDown(CDC *pDC, CCAMCView *pView, CPoint point);
	void GetSelectedPixels(COLORREF& c);
	COLORREF GetMeanColor();
	void SetMeanColor();
private:
	COLORREF m_meancolor;
};
