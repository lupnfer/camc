#pragma once
#include "basicgridctrl.h"

class COutputWnd :
	public CBasicGridCtrl
{
public:
	COutputWnd(void);
	~COutputWnd(void);
public:
	virtual void InitGridColumn();
	virtual void OnSelChanged(const CBCGPGridRange&range,BOOL bSelect);
};
