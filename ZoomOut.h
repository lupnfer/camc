#pragma once
#include "unit.h"
#include "CAMCView.h"
class CZoomOut :
	public CUnit
{
public:
	CZoomOut(void);
	~CZoomOut(void);
	virtual void OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point);


	virtual void SetZoomRate(int rate)
	{
		m_zoomrate = rate;
	}
	int GetZoomRate()
	{
		return m_zoomrate;
	}
	static CZoomOut* Instance();
private:
	int  m_zoomrate;
	static CZoomOut* _instance;
};
