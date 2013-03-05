#pragma once
#include "unit.h"

class CErase :
	public CUnit
{
public:
	CErase(void);
	~CErase(void);
	virtual void OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point);
	static CErase* Instance();
private:

	static CErase* _instance;
};
