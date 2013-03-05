#pragma once
#include "unit.h"


class CZoomIn :
	public CUnit
{
public:
	CZoomIn(void);
	~CZoomIn(void);
	virtual void OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void SetZoomRate(int rate)
	{
		m_zoomrate = rate;
	}
	int GetZoomRate()
	{
		return m_zoomrate;
	}
	static CZoomIn* Instance();
private:
	int m_zoomrate;
	static CZoomIn* _instance;
};
