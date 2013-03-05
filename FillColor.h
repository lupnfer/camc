#pragma once
#include "unit.h"

class CFillColor :
	public CUnit
{
public:
	CFillColor(void);
	~CFillColor(void);
	virtual void OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point);

	static CFillColor* Instance();
private:
	static CFillColor* _instance;
};
