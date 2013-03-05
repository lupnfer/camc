#pragma once
#include "objbase.h"

class CParentBrush :
	public FCObjGraph
{
public:
	CParentBrush(void);
	~CParentBrush(void);
	int Radio(){return m_r;}
	
protected:
	int    m_r;
};
